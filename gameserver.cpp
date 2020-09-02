#include "gameserver.h"

GameServer::GameServer(QObject *parent) : QObject(parent)
{
//    QJsonObject root, server;
//    QJsonDocument doc;
//    QFile file("config.json");
//    doc.fromJson(file.readAll());
//    doc.fromJson("{ \"server\":{ \"IP\" : \"127.0.0.1\", \"port\" : 12345, \"notice\" : \"null\" } }");
//    root = doc.object();
//    server = root["server"].toObject();
//    this->notice = server["notice"].toString();
//    if(listener.listen(QHostAddress(server["IP"].toString()), server["port"].toInt()))
//        cout << "Server is LISTENING..." << endl;
    if(listener.listen(QHostAddress("127.0.0.1"), 12345))
        cout << "Server is LISTENING..." << endl;
    connect(&listener, &QTcpServer::newConnection, this, &GameServer::onNewConnection);
    this->notice = "test!!!";
}

void GameServer::onReadyRead()
{
    Msg msg;
    QTcpSocket *clientSocket = (QTcpSocket *)sender();
    receiveFromClient(msg, clientSocket);
    msgHandler(msg, clientSocket);
}

void GameServer::onDisconnected()
{
    cout << "A Connector has disconnected" << endl;
    onlineList.remove((QTcpSocket *)sender());
}

void GameServer::onNewConnection()
{
    QTcpSocket *p = listener.nextPendingConnection();

    cout << "Get A New Connection..." << endl;
    cout << p->peerAddress().toString().toUtf8().data() << endl;

    onlineList[p] = "";
    connect(p, &QTcpSocket::readyRead, this, &GameServer::onReadyRead);
    connect(p, &QTcpSocket::disconnected, this, &GameServer::onDisconnected);
    Msg msg(connectSuccess, this->notice);
    sendToClient(msg, p);
}

void GameServer::sendToClient(Msg &msg, QTcpSocket *socket)
{
    QString data = msg.packUp();
    socket->write(data.toUtf8().data(), data.size());

#if DEBUGMODE == true
    cout << "Send A New Msg..." << endl;
    cout << "type:" << msg.type << endl;
    cout << "content:" << msg.content.toUtf8().data() << endl;
#endif

}

void GameServer::receiveFromClient(Msg &msg, QTcpSocket *socket)
{
    QString data = socket->readAll();
    msg.unpack(data.toUtf8().data());

#if DEBUGMODE == true
    cout << "Get A New Msg..." << endl;
    cout << "type:" << msg.type << endl;
    cout << "content:" << msg.content.toUtf8().data() << endl;
#endif

}

void GameServer::msgHandler(Msg &msg, QTcpSocket *sender)
{
    switch((int)msg.type)
    {
    case msgType::matchQuery:
    {
        if(matchingQueue.isEmpty())
            matchingQueue.push_back(onlineList[sender]);
        else
            return;
        /*待定*/
        break;
    }
    case msgType::userName:
    {
        onlineList[sender] = msg.content;
        cout << "A user has connected." << endl;
        break;
    }
    case msgType::uploadScore:
    {
        cout << "A user has sent score : " << msg.content.toUtf8().data() << endl;
        QJsonObject obj = QJsonDocument::fromJson(msg.content.toUtf8()).object();
        db.insertScore(onlineList[sender], obj["score"].toInt(), obj["maxNum"].toInt());
        break;
    }
    case msgType::personalAchievementQuery:
    {
        cout << "A user is querying personal achievement. " << endl;
        QJsonDocument doc;
        doc.setArray(QJsonArray::fromVariantList(db.queryScore(onlineList[sender])));
        Msg msg(msgType::personalAchievement, doc.toJson());
        sendToClient(msg, sender);
        break;
    }
    case msgType::allAchievementQuery:
    {
        cout << "A user is querying all achievement. " << endl;
        QJsonDocument doc;
        doc.setArray(QJsonArray::fromVariantList(db.queryScore()));
        Msg msg(msgType::allAchievement, doc.toJson());
        sendToClient(msg, sender);
        break;
    }
    }
}
