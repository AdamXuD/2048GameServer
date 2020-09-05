#include "common.h"

#include "gameserver.h"
#include "database.h"
#include "QJsonArray"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    GameServer gs;
//    QFile file("test.txt");
//    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
//        cout << "niupi" << endl;
//    file.write("niupi");
//    file.close();
//    a.exit();
    return a.exec();
}
