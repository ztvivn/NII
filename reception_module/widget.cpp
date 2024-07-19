#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 49101);
    connect(udpSocket, &QUdpSocket::readyRead, this, &Widget::udpServer);
    connect(ui->pushButton_close, &QPushButton::clicked, this, &Widget::pushButton_close_clicked);//соединяем сигнал кнопки закрытия со слотом
}

Widget::~Widget()
{
    delete ui;
}

void Widget::udpServer()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        data_array = deserialize(datagram);
        ui->doubleSpinBox_latitude->setValue(data_array.latitude);//заполняем данными из пришедших пакетов графический интерфейс
        ui->doubleSpinBox_longitude->setValue(data_array.longitude);
        ui->doubleSpinBox_altitude->setValue(data_array.altitude);
        ui->doubleSpinBox_course->setValue(data_array.course);
        ui->lineEdit_name->setText(data_array.getName());
        ui->lineEdit_description->setText(data_array.getDescription());

        if(data_array.value_n == 4)//начинаем заполнять таблицу после того, как придет последний 4 пакет
        {
            ui->lineEdit_data->setText(data_array.getData());//выводим на экран информацию в поле "Класс объекта"

            ui->tableWidget->setRowCount(64);//строк в табл
            ui->tableWidget->setColumnCount(64);//столбцов в табл

            for (int row = 0; row < 64; row++)//заполняем таблицу данными из массива data
            {
                for (int col = 0; col < 64; col++)
                {
                    int index = row * 64 + col;
                    if (index < 4096)
                    {
                        int number = data_array.data[index];// Преобразуем байт в число для отображения
                        QTableWidgetItem *item = new QTableWidgetItem(QString::number(number, 16).rightJustified(2, '0'));
                        ui->tableWidget->setItem(row, col, item);//записываем данные в каждую ячейку
                    }
                }
            }
            ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//запрещаем редактирование таблицы
        }
    }
}

void Widget::pushButton_close_clicked()
{
    close();//безопасное закрытие программы
}

