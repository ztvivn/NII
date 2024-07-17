#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include <QByteArray>
#include <array>

#define DISCHARGE_PRICE 0.01//цена младшего разряда
#define MASK_LATITUDE 16384//14 бит для минуса широты
#define MASK_LONGITUDE 32768//15 бит для минуса долготы
#define MASK_ALTITUDE 2097152//21 бит для минуса высоты
#define MASK_COURSE 65536//16 бит для минуса курса

struct Data_block
{
private:
    uint32_t  latitude;//широта
    uint32_t longitude;//долгота
    uint32_t altitude;//высота
    uint32_t course;//курс
    std::array<char, 8> name;//имя объекта
    std::array<char, 32> description;//описание объекта
public:
    unsigned char data[4096];// доп данные
    Data_block();

    void setLatitude(double value);//заполнене широты
    uint32_t getLatitude();//вывод широты

    void setLongitude(double value);//инициализация долготы
    uint32_t getLongitude();//получить значение долготы

    void setAltitude(double value);//инициализация высоты
    uint32_t getAltitude();//получение значения высоты

    void setCourse(double value);//инициализация курса
    uint32_t getCourse();//получение значения курса

    void setName(QString text);//инициализация имени объекта
    std::array<char, 8> getName();//получение имени объекта в виде массива символов

    void setDescription(QString text);//инициализация описания объекта
    std::array<char, 32> getDescription();//получение описания объекта

    QString getData();//возращаем последние 4 байта из массива data(полезная для нас информация)

    double accuracy(double value);//округляет до 3 знаков после запятой
    uint32_t encryption(double value, const double discharge_price, const uint32_t mask);//переводит широту в целочисленный тип и ставит побитово маску для знака
};

#endif // DATA_BLOCK_H
