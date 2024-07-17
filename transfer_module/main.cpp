#include "widget.h"
#include "client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    w.setWindowTitle("Модуль передачи данных");
    w.show();

    return a.exec();
}
