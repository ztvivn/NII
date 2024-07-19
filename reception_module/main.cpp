#include "widget.h"
#include "data_block.h"
#include <QApplication>
#include <QUdpSocket>
#include <QNetworkDatagram>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    w.show();
    w.udpServer();

    return a.exec();
}
