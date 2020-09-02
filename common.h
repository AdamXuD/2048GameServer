#ifndef COMMON_H
#define COMMON_H

#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QQueue>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QArrayData>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <iostream>
#include <memory>

#define DEBUGMODE true

using namespace std;

enum msgType{connectSuccess, userName, uploadScore, matchQuery, personalAchievementQuery, allAchievementQuery, matchSuccess, personalAchievement, allAchievement};

const QString _MsgHeader = "_MSGHEADER_";
const QString _MsgEnd = "_MSGEND_";

class Msg
{
public:
    msgType type;
    QString content;

public:
    Msg(msgType type, QString content = "") : type(type), content(content) { }
    Msg() { }
    QString packUp() { return _MsgHeader + serializer() + _MsgEnd; }
    void unpack(QString data) { if(data.indexOf(_MsgHeader) != -1 && data.indexOf(_MsgEnd) != -1) parser(data.remove(_MsgHeader).remove(_MsgEnd)); }


private:
    void parser(const QString &data)
    {
        QJsonDocument doc;
        QJsonObject root = doc.fromJson(data.toUtf8()).object();
        this->type = (msgType)root["type"].toInt();
        this->content = root["content"].toString();
    }

    QString serializer()
    {
        QJsonObject root;
        root.insert("type", type);
        root.insert("content", content);
        QJsonDocument doc;
        doc.setObject(root);
        return doc.toJson();
    }

};

#endif // COMMON_H
