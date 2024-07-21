#ifndef DESERIALIZATION_H
#define DESERIALIZATION_H

#include <array>
#include <QNetworkDatagram>

struct Data_block
{
    //входящий пакет с данными
    unsigned char data[4096];
    //широта
    double  latitude;
    //долгота
    double longitude;
    //высота
    double altitude;
    //курс
    double course;
    //имя объекта
    std::array<char, 8> name;
    //описание объекта
    std::array<char, 32> description;
    //порядковый номер фрагмента информации
    uint8_t value_n;
};

//десериализация данных
Data_block deserialize(const QByteArray &data);
//возвращает 4092 байт из массива на графическую чать в поле часть объекта
uint32_t getClass(const Data_block &data);
//возвращает имя объекта
QString getName(Data_block data_array);
//возвразает описание объекта
QString getDescription(Data_block data_array);
//переводим строку name в кодировку Win1251
QString convertFromWindows1251Name(const std::array<char, 8> &inputArray);
//переводим строку description в кодировку Win1251
QString convertFromWindows1251Description(const std::array<char, 32> &inputArray);

#endif // DESERIALIZATION_H
