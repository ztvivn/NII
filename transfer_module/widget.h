#ifndef WIDGET_H
#define WIDGET_H

#include "data_block.h"
#include "client.h"
#include <QWidget>
#include <thread>

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

private slots:
    void pushButton_close_clicked();//закрытие программы

    void pushButton_send_clicked();//загрузка данных и ее отправка

private:
    Ui::Widget *ui;
    Data_block data;
};
#endif // WIDGET_H
