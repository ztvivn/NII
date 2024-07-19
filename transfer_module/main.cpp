#include <QApplication>

#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    char data_array[4096];

    w.setData(data_array);
    w.show();

    return a.exec();
}
