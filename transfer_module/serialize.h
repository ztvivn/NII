#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <QByteArray>
#include <array>

//цена младшего разряда
#define DISCHARGE_PRICE 0.01
//14 бит для минуса широты
#define MASK_LATITUDE 16384
//15 бит для минуса долготы
#define MASK_LONGITUDE 32768
//21 бит для минуса высоты
#define MASK_ALTITUDE 2097152
//16 бит для минуса курса
#define MASK_COURSE 65536

struct Data_block
{
    //блок с данными
    char data[4096];
    //широта
    uint32_t  latitude;
    //долгота
    uint32_t longitude;
    //высота
    uint32_t altitude;
    //курс
    uint32_t course;
    //имя объекта
    std::array<char, 8> name;
    //описание объекта
    std::array<char, 32> description;
};

//инициализация имени объекта
void setName(Data_block &data, QString text);
//инициализация описания объекта
void setDescription(Data_block &data, QString text);

//возращаем последние 4 байта из массива data(полезная для нас информация)
QString getData(Data_block &data);
//округляет до 3 знаков после запятой
double accuracy(double value);
//переводит широту в целочисленный тип и ставит побитово маску для знака
uint32_t encryption(double value, const double discharge_price, const uint32_t mask);
//метод для преобразования в кодировку Win1251 для name
std::array<char, 6> convertToWindows1251Name(const QString &str);
//метод для преобразования в кодировку Win1251 для description
std::array<char, 32> convertToWindows1251Description(const QString &str);

//сериализация пакета
QByteArray serialize(Data_block &data, uint package_number);
//алгоритм crc16
uint16_t crc16(unsigned char *array, uint lenght);

//формирование заголовка пакета
void serialize_heading(Data_block &data, QDataStream &stream);
//формирование атрибутов пакета
void serialize_attribut(Data_block &data, QDataStream &stream, QByteArray &buffer, uint package_number);
//формирование пакета блока данных
void serialize_data(Data_block &data, QDataStream &stream, uint package_number);

#endif // SERIALIZE_H
