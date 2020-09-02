#ifndef DATABASE_H
#define DATABASE_H

#include "common.h"

class Database
{
    QSqlDatabase *db;
public:
    Database();
    ~Database();

    void insertScore(QString name, int score, int maxNum);
    QVariantList queryScore(QString name = "");

};

#endif // DATABASE_H
