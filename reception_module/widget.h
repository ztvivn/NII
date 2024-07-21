#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>

#include "deserialization.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    Data_block data_array;
    //вывод полученной информации из 4 пакетов от модуля отправки данных
    void acceptData();

private slots:
    //закрываем программу
    void on_pushButton_close_clicked();

private:
    Ui::Widget *ui;
    QUdpSocket *udpSocket;
};
#endif // WIDGET_H
