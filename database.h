#ifndef DATABASE_H
#define DATABASE_H

#include "common.h"

class Database
{
    QSqlDatabase *db;
    std::function<void(QString)> logPrinter;


public:
    Database();
    ~Database();

    void init(std::function<void(QString)> func);

    void insertScore(QString name, int score, int maxNum);
    QVariantList queryScore(QString name = "");
private:
    QSqlQuery queryExec(QString query);
};

#endif // DATABASE_H
