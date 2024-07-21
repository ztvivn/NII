#include "widget.h"
#include "./ui_widget.h"

#include <fstream>

#define PORT 49101

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, PORT);
    connect(udpSocket, &QUdpSocket::readyRead, this, &Widget::acceptData);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::acceptData()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        data_array = deserialize(datagram);
        //заполняем данными из пришедших пакетов графический интерфейс
        ui->doubleSpinBox_latitude->setValue(data_array.latitude);
        ui->doubleSpinBox_longitude->setValue(data_array.longitude);
        ui->doubleSpinBox_altitude->setValue(data_array.altitude);
        ui->doubleSpinBox_course->setValue(data_array.course);
        ui->lineEdit_name->setText(getName(data_array));
        ui->lineEdit_description->setText(getDescription(data_array));

        //начинаем заполнять таблицу после того, как придет последний 4 пакет
        if(data_array.value_n == 4)
        {
            //выводим на экран информацию в поле "Класс объекта"
            ui->lineEdit_data->setText(QString::number(getClass(data_array)));

            //строк в табл
            ui->tableWidget->setRowCount(64);
            //столбцов в табл
            ui->tableWidget->setColumnCount(64);

            //заполняем таблицу данными из массива data
            for (int row = 0; row < 64; row++)
            {
                for (int col = 0; col < 64; col++)
                {
                    int index = row * 64 + col;
                    if (index < 4096)
                    {
                        // Преобразуем байт в число для отображения
                        int number = static_cast<unsigned char>(data_array.data[index]);
                        QTableWidgetItem *item = new QTableWidgetItem(QString::number(number, 16).rightJustified(2, '0'));
                        //записываем данные в каждую ячейку
                        ui->tableWidget->setItem(row, col, item);
                    }
                }
            }
            //запрещаем редактирование таблицы
            ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        }
    }
}

void Widget::on_pushButton_close_clicked()
{
    //безопасное закрытие программы
    close();
}

