#include "common.h"

#include "gameserver.h"
#include "database.h"
#include "QJsonArray"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    GameServer gs;
//    Database db;
    return a.exec();
}
