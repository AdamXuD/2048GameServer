#ifndef GAMESERVER_H
#define GAMESERVER_H

#include "common.h"
#include "database.h"
#include "logthread.h"
#include "task.h"

class GameServer : public QObject
{
    Q_OBJECT
    QTcpServer listener;

    QQueue<QString> matchingQueue;
    QMap<QTcpSocket *, QString> onlineList;

    QString notice;
    Database db;
    QThreadPool pool;
    QMutex mutex;
    LogThread log;
    bool debugMode = true;

public:
    explicit GameServer(QObject *parent = nullptr);

signals:
    void sendSignal(QTcpSocket *socket, msgType type, QString content);

public slots:
    void onNewConnection();
    void onDisconnected();
    void onReadyRead();
    void onSendSignal(QTcpSocket *socket, msgType type, QString content = QString());

private:
    void receiveFromClient(Msg &msg, QTcpSocket *socket);
    void sendToClient(Msg &msg, QTcpSocket *socket);
    void msgHandler(Msg &msg, QTcpSocket *sender);


};

#endif // GAMESERVER_H

