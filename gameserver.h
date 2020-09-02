#ifndef GAMESERVER_H
#define GAMESERVER_H

#include "common.h"
#include "database.h"

class GameServer : public QObject
{
    Q_OBJECT
    QTcpServer listener;
    QQueue<QString> matchingQueue;
    QMap<QTcpSocket *, QString> onlineList;
    QString notice;
    Database db;

public:
    explicit GameServer(QObject *parent = nullptr);

signals:

public slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void receiveFromClient(Msg &msg, QTcpSocket *socket);
    void sendToClient(Msg &msg, QTcpSocket *socket);

private:
    void msgHandler(Msg &msg, QTcpSocket *sender);
};

#endif // GAMESERVER_H
