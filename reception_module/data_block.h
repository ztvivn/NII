#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include <array>
#include <QByteArray>
#include <QApplication>
#include <QUdpSocket>
#include <QNetworkDatagram>

struct Data_block
{
    Data_block();
    unsigned char data[4096];
    double  latitude;//широта
    double longitude;//долгота
    double altitude;//высота
    double course;//курс
    std::array<char, 8> name;//имя объекта
    std::array<char, 32> description;//описание объекта
    uint8_t value_n;//порядковый номер фрагмента информации
    QString getData();
    QString getName();
    QString getDescription();
    QString convertFromWindows1251Name(const std::array<char, 8> &inputArray);//переводим строку name в кодировку Win1251
    QString convertFromWindows1251Description(const std::array<char, 32> &inputArray);//переводим строку description в кодировку Win1251
};

Data_block deserialize(const QByteArray &data);

#endif // DATA_BLOCK_H
