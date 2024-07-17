#ifndef WIDGET_H
#define WIDGET_H

#include "data_block.h"
#include <QWidget>
#include <QUdpSocket>

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

    void udpServer();

private slots:
    void pushButton_close_clicked();//закрываем программу

private:
    Ui::Widget *ui;
    QUdpSocket *udpSocket;
};
#endif // WIDGET_H
