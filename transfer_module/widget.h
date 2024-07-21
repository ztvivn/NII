#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "serialize.h"

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
    void setData(std::string file_name);

private slots:
    //закрытие программы
    void on_pushButton_close_clicked();
    //загрузка данных и ее отправка
    void on_pushButton_send_clicked();

private:
    Ui::Widget *ui;
    Data_block data;
};
#endif // WIDGET_H
