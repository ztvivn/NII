#include "data_block.h"
#include <QString>
#include <QByteArray>
#include <array>
#include <cmath>

Data_block::Data_block()
{
    data[4092] = 4;
    data[4093] = 0;
    data[4094] = 0;
    data[4095] = 0;
}

double Data_block::accuracy(double value)
{
    return std::round(value * 100.0f) / 100.0f;
}

uint32_t Data_block::encryption(double value, const double discharge_price, const uint32_t mask)
{
    uint32_t value_integer;//целочисленное представление

    if(value < 0)//если число отрицательное
    {
        value = std::fabs(value);//убираем минус
        value_integer = value / discharge_price;//переводим в целочисленный вид
        value_integer = mask | value_integer;//ставим маску для минуса(самый левый бит)

        return value_integer;
    }

    value_integer = value / discharge_price;//переводим в целочисленный вид

    return value_integer;//возвращаем зашифрованное значение
}

void Data_block::setLatitude(double value)
{
    value = accuracy(value);
    this->latitude = encryption(value, DISCHARGE_PRICE, MASK_LATITUDE);
}

uint32_t Data_block::getLatitude()
{
    return this->latitude;
}

void Data_block::setLongitude(double value)
{
    value = accuracy(value);
    this->longitude = encryption(value, DISCHARGE_PRICE, MASK_LONGITUDE);
}

uint32_t Data_block::getLongitude()
{
    return this->longitude;
}

void Data_block::setAltitude(double value)
{
    value = accuracy(value);
    this->altitude = encryption(value, DISCHARGE_PRICE, MASK_ALTITUDE);
}

uint32_t Data_block::getAltitude()
{
    return this->altitude;
}

void Data_block::setCourse(double value)
{
    value = accuracy(value);
    this->course = encryption(value, DISCHARGE_PRICE, MASK_COURSE);
}

uint32_t Data_block::getCourse()
{
    return this->course;
}

void Data_block::setName(QString text)
{
    std::string text_copy;
    text_copy = text.toStdString();

    for(int i = 0; i < text_copy.size(); i++)
    {
        this->name[i] = text_copy[i];
    }

    for(int i = text_copy.size(); i < 6; i++)
    {
        this->name[i] = '\0';
    }
}

std::array<char, 8> Data_block::getName()
{
    return this->name;
}

void Data_block::setDescription(QString text)
{
    std::string text_copy;
    text_copy = text.toStdString();

    for(int i = 0; i < text_copy.size(); i++)
    {
        this->description[i] = text_copy[i];
    }

    for(int i = text_copy.size(); i < 32; i++)
    {
        this->description[i] = '\0';
    }
}

std::array<char, 32> Data_block::getDescription()
{
    return this->description;
}

QString Data_block::getData()
{
    uint32_t value;
    QString value_str;

    value = *reinterpret_cast<uint32_t*>(&data[4092]);
    value_str = QString::number(value);//переводим значение в строку, чтобы вывести ее на экран

    return value_str;
}
