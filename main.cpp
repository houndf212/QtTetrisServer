#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    quint16 port = 2333;
    if (!a.arguments().isEmpty())
    {
        QString s = a.arguments().at(0);
        bool b = false;
        quint16 t = s.toUInt(&b);
        if (b) port = t;

    }
    Server s(port);
    return a.exec();
}
