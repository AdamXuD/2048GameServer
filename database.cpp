#include "database.h"

Database::Database()
{
    this->db = new QSqlDatabase;
    *db = QSqlDatabase::addDatabase("QSQLITE");
    db->setDatabaseName(QCoreApplication::applicationDirPath() + "/gamedata.dat");
    if(!db->open())
        cout << "QDatabase Error : " << db->lastError().text().toUtf8().data() << endl;
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS tableScore (uid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, uname VARCHAR NOT NULL DEFAULT ('_unknown_'), score INTEGER DEFAULT (0), maxNum INTEGER NOT NULL DEFAULT (0), uploadTime DATETIME NOT NULL DEFAULT (datetime('now', 'localtime')));");
}

Database::~Database()
{
    delete this->db;
}

void Database::insertScore(QString name, int score, int maxNum)
{
    QSqlQuery query;
    QString command = QString("INSERT INTO tableScore VALUES (null, '%1', %2, %3, null);").arg(name).arg(score).arg(maxNum);
    if(!query.exec(command))
        cout << "SQL query ERROR at " << command.toUtf8().data() << " : " << db->lastError().text().toUtf8().data() << endl;
}

QVariantList Database::queryScore(QString name)
{
    QSqlQuery query;
    QString command;
    if(name.isEmpty())
        command = QString("SELECT uname, score, maxNum, uploadTime FROM tableScore ORDER BY score DESC LIMIT 0,10;");
    else
        command = QString("SELECT uname, score, maxNum, uploadTime FROM tableScore WHERE uname = '%1' ORDER BY score DESC;").arg(name);
    if(!query.exec(command))
    {
        cout << "SQL query ERROR at " << command.toUtf8().data() << " : " << db->lastError().text().toUtf8().data() << endl;
        return QVariantList();
    }
    if(query.size() == 0)
        return QVariantList();

    QVariantList resultList;
    QVariantMap tmpObj;
    while(query.next())
    {
        tmpObj.clear();

        tmpObj["uname"] = query.value(0).toString();
        tmpObj["score"] = query.value(1).toInt();
        tmpObj["maxNum"] = query.value(2).toInt();
        tmpObj["uploadTime"] = query.value(3).toString();
        resultList.push_back(tmpObj);
    }
    return resultList;
}
