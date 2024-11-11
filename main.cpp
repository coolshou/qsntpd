#include <QCoreApplication>
#include "src/sntpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    SNTPServer server;
    return a.exec();
}
