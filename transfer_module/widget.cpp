#include "widget.h"

#include <fstream>

#include "./ui_widget.h"
#include "client.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_close_clicked()
{
    //безопасное закрытие программы
    close();
}


void Widget::on_pushButton_send_clicked()
{
    //считваем значение для широты
    data.latitude = encryption(ui->doubleSpinBox_latitude->value(), DISCHARGE_PRICE, MASK_LATITUDE);
    //считываем значение для долготы
    data.longitude = encryption(ui->doubleSpinBox_longitude->value(), DISCHARGE_PRICE, MASK_LONGITUDE);
    //считываем значение для высоты
    data.altitude = encryption(ui->doubleSpinBox_altitude->value(), DISCHARGE_PRICE, MASK_ALTITUDE);
    //считываем значение для курса
    data.course = encryption(ui->doubleSpinBox_course->value(), DISCHARGE_PRICE, MASK_COURSE);

    //считываем строку имени объекта
    setName(data, ui->lineEdit_name->text());
    //считываем строку описание объекта
    setDescription(data, ui->lineEdit_description->text());

    //проверка на пустые поля
    if(ui->lineEdit_name->text().isEmpty() || ui->lineEdit_description->text().isEmpty())
    {
        ui->error_output->setText("\t\t\tОшибка! Вы не заполнили все поля!");
        ui->error_output->setStyleSheet("QLabel { color : red; }");
        return;
    }

    ui->error_output->setText("\t\t\tCообщение успешно отправленно!");
    ui->error_output->setStyleSheet("QLabel { color : green; }");

    Client client;
    //отправляем 4 пакета с нашей информацией
    for(uint i = 1; i <= 4; ++i)
    {
        client.send(serialize(data, i));
    }
}

void Widget::setData(std::string file_name)
{
    //открываем файл с данными 4кб
    std::ifstream file_array(file_name, std::ios::binary);
    //проверка на наличие файла
    if(!file_array)
    {
        qDebug() << "no file";
        close();
    }
    //читаем файл
    file_array.read(reinterpret_cast<char*>(data.data), sizeof(data.data));
    //закрываем файл
    file_array.close();

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
                int number = static_cast<unsigned char>(data.data[index]);
                QTableWidgetItem *item = new QTableWidgetItem(QString::number(number, 16).rightJustified(2, '0'));
                //записываем данные в каждую ячейку
                ui->tableWidget->setItem(row, col, item);
            }
        }
    }
    //запрещаем редактирование таблицы
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //выводим на экран информацию в поле "Класс объекта"
    ui->lineEdit_data->setText(QString::number(getClass(data)));
}
