#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->pushButton_close, &QPushButton::clicked, this, &Widget::pushButton_close_clicked);//соединяем сигнал кнопки закрытия со слотом
    connect(ui->pushButton_send, &QPushButton::clicked, this, &Widget::pushButton_send_clicked);//соединяем сигнал отправки данных

    ui->lineEdit_data->setText(data.getData());//выводим на экран информацию в поле "Класс объекта"

    ui->tableWidget->setRowCount(64);//строк в табл
    ui->tableWidget->setColumnCount(64);//столбцов в табл

    for (int row = 0; row < 64; row++)//заполняем таблицу данными из массива data
    {
        for (int col = 0; col < 64; col++)
        {
            int index = row * 64 + col;
            if (index < 4096)
            {
                int number = data.data[index];// Преобразуем байт в число для отображения
                QTableWidgetItem *item = new QTableWidgetItem(QString::number(number, 16).rightJustified(2, '0'));
                ui->tableWidget->setItem(row, col, item);//записываем данные в каждую ячейку
            }
        }
    }
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//запрещаем редактирование таблицы
}

Widget::~Widget()
{
    delete ui;
}

void Widget::pushButton_close_clicked()
{
    qApp->quit();//безопасное закрытие программы
}


void Widget::pushButton_send_clicked()
{
    data.setLatitude(ui->doubleSpinBox_latitude->value());//считваем значение для широты
    data.setLongitude(ui->doubleSpinBox_longitude->value());//считываем значение для долготы
    data.setAltitude(ui->doubleSpinBox_altitude->value());//считываем значение для высоты
    data.setCourse(ui->doubleSpinBox_course->value());//считываем значение для курса

    data.setName(ui->lineEdit_name->text());//считываем строку имени объекта
    data.setDescription(ui->lineEdit_description->text());//считываем строку описание объекта

    if(ui->lineEdit_name->text().isEmpty() || ui->lineEdit_description->text().isEmpty())//проверка на пустые поля
    {
        ui->error_output->setText("\t\t\tОшибка! Вы не заполнили все поля!");
        ui->error_output->setStyleSheet("QLabel { color : red; }");
        return;
    }

    ui->error_output->setText("\t\t\tCообщение успешно отправленно!");
    ui->error_output->setStyleSheet("QLabel { color : green; }");

    Client client;

    for(uint i = 1; i <= 4; ++i)
    {
        client.send(client.serialize(data, i));
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));//отправляем сообщение 1 раз в секунду
    }
}

