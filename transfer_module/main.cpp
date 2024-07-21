#include <QApplication>

#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    //записываем данные в блок данных 4кб из файла с именем в параметрах
    w.setData("information.bin");
    w.show();

    return a.exec();
}
