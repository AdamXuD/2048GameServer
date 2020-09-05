#include "gameserver.h"

GameServer::GameServer(QObject *parent) : QObject(parent)
{
    log.start();
    db.init(std::bind(&LogThread::printLog, &log, logType::database, std::placeholders::_1));

    QFile config("config.json");
    if(!config.open(QIODevice::ReadOnly))
    {
        log.printLog(logType::socket, "config.json open failed. Is it existed?");
        return;
    }

    QJsonObject root, server;
    QJsonDocument doc = QJsonDocument::fromJson(config.readAll());

    config.close();

    server = doc.object()["server"].toObject();
    this->notice = server["notice"].toString();
    QString IP = server["IP"].toString();
    unsigned int port = server["port"].toInt();
    unsigned int maxThreadCount = server["maxThreadCount"].toInt();

    if(listener.listen(QHostAddress(IP), port))
        log.printLog(logType::socket, "Server is LISTENING on " + QString("%1:%2").arg(IP).arg(port) + "...");

    connect(&listener, &QTcpServer::newConnection, this, &GameServer::onNewConnection);
    qRegisterMetaType<Msg>("Msg");
    qRegisterMetaType<msgType>("msgType");
    connect(this, &GameServer::sendSignal, this, &GameServer::onSendSignal);

    pool.setMaxThreadCount(maxThreadCount);
    log.printLog(logType::threadpool, QString("Threadpool initialization has finished, maximum thread Count = %1").arg(maxThreadCount));

    log.printLog(logType::socket, QString("Server V%1 start successfully.").arg(VERSION));
}

void GameServer::onNewConnection()
{
    QTcpSocket *p = listener.nextPendingConnection();

    log.printLog(logType::socket, "Get A New Connection:" + p->peerAddress().toString());

    {
        QMutexLocker locker(&this->mutex);
        onlineList[p] = "";
    }
    connect(p, &QTcpSocket::readyRead, this, &GameServer::onReadyRead);
    connect(p, &QTcpSocket::disconnected, this, &GameServer::onDisconnected);
    emit sendSignal(p, connectSuccess, this->notice);
}

void GameServer::onDisconnected()
{
    log.printLog(logType::socket, "A Connector has disconnected.");
    {
        QMutexLocker locker(&this->mutex);
        onlineList.remove((QTcpSocket *)sender());
    }
}


void GameServer::onReadyRead()
{
    Msg msg;
    QTcpSocket *clientSocket = (QTcpSocket *)sender();
    receiveFromClient(msg, clientSocket);
    msgHandler(msg, clientSocket);
}

void GameServer::onSendSignal(QTcpSocket *socket, msgType type, QString content)
{
    Msg msg(type, content);
    sendToClient(msg, socket);
}

void GameServer::sendToClient(Msg &msg, QTcpSocket *socket)
{
    QString data = msg.packUp();
    socket->write(data.toUtf8().data(), data.size());

    if(this->debugMode == true)
    {
        log.printLog(logType::socket, "Send A New Msg.");
        log.printLog(logType::socket, "type:" + QString("%1").arg((int)msg.type));
        log.printLog(logType::socket, "content:" + msg.content);
    }
}

void GameServer::receiveFromClient(Msg &msg, QTcpSocket *socket)
{
    QString data = socket->readAll();
    msg.unpack(data.toUtf8().data());

    if(this->debugMode == true)
    {
        log.printLog(logType::socket, "Get A New Msg.");
        log.printLog(logType::socket, "type:" + QString("%1").arg((int)msg.type));
        log.printLog(logType::socket, "content:" + msg.content);
    }
}

void GameServer::msgHandler(Msg &msg, QTcpSocket *sender)
{
    Task *task = new Task([=](){
        switch((int)msg.type)
        {
        case msgType::matchQuery:
        {
            QMutexLocker lock(&this->mutex);
            if(matchingQueue.isEmpty())
                matchingQueue.push_back(onlineList[sender]);
            else
                return;
            /*待定*/
            break;
        }
        case msgType::userName:
        {
            QMutexLocker lock(&this->mutex);
            onlineList[sender] = msg.content;
            log.printLog(logType::socket, "A user has connected.");
            break;
        }
        case msgType::uploadScore:
        {
            QString senderName;
            {
                QMutexLocker lock(&this->mutex);
                senderName = onlineList[sender];
            }
            log.printLog(logType::socket, "A user has sent score.");
            QJsonObject obj = QJsonDocument::fromJson(msg.content.toUtf8()).object();
            db.insertScore(senderName, obj["score"].toInt(), obj["maxNum"].toInt());
            break;
        }
        case msgType::personalAchievementQuery:
        {
            QString senderName;
            {
                QMutexLocker lock(&this->mutex);
                senderName = onlineList[sender];
            }
            log.printLog(logType::socket, "A user is querying personal achievement. ");
            QJsonDocument doc;
            doc.setArray(QJsonArray::fromVariantList(db.queryScore(senderName)));
            emit sendSignal(sender, msgType::personalAchievement, doc.toJson());
            break;
        }
        case msgType::allAchievementQuery:
        {
            log.printLog(logType::socket, "A user is querying all achievement. ");
            QJsonDocument doc;
            doc.setArray(QJsonArray::fromVariantList(db.queryScore()));
            emit sendSignal(sender, msgType::allAchievement, doc.toJson());
            break;
        }
        }
    }, std::bind(&LogThread::printLog, &log, logType::threadpool, std::placeholders::_1));

    pool.start(task);

}
