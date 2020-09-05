#include "database.h"

Database::Database()
{
    this->db = new QSqlDatabase;
}

Database::~Database()
{
    delete this->db;
}

void Database::init(std::function<void(QString)> func)
{
    this->logPrinter = func;

    *db = QSqlDatabase::addDatabase("QSQLITE");
    db->setDatabaseName(QCoreApplication::applicationDirPath() + "/gamedata.dat");
    if(!db->open())
        logPrinter("QDatabase Error : " + db->lastError().text());
    else
        logPrinter("Database open success.");
    queryExec("CREATE TABLE IF NOT EXISTS tableScore (uid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, uname VARCHAR NOT NULL DEFAULT ('_unknown_'), score INTEGER DEFAULT (0), maxNum INTEGER NOT NULL DEFAULT (0), uploadTime DATETIME NOT NULL DEFAULT (datetime('now', 'localtime')));");
    logPrinter("Database initialize finished.");
}

void Database::insertScore(QString name, int score, int maxNum)
{
    queryExec(QString("INSERT INTO tableScore VALUES (null, '%1', %2, %3, datetime('now', 'localtime'));").arg(name).arg(score).arg(maxNum));
}

QVariantList Database::queryScore(QString name)
{
    QString command;
    if(name.isEmpty())
        command = QString("SELECT uname, score, maxNum, uploadTime FROM tableScore ORDER BY score DESC LIMIT 0,10;");
    else
        command = QString("SELECT uname, score, maxNum, uploadTime FROM tableScore WHERE uname = '%1' ORDER BY score DESC;").arg(name);

    QSqlQuery query = queryExec(command);

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

QSqlQuery Database::queryExec(QString query)
{
    QSqlQuery sqlQuery;
    if(!sqlQuery.exec(query))
        logPrinter("SQL query ERROR at " + query + " : " + db->lastError().text());
    return sqlQuery;
}
