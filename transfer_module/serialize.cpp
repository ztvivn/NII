#include "serialize.h"

#include <iconv.h>
#include <QDebug>
#include <QIODevice>

uint32_t encryption(float value, const float discharge_price, const uint32_t mask)
{
    //целочисленное представление
    uint32_t value_integer;

    //если значение не отрицательно, маску не добавляем
    if(value >= 0)
    {
        value_integer = value / discharge_price;
    }else//блок для отрицательных значений, ставим маску
    {
        //убираем минус
        value = std::fabs(value);
        //переводим в целочисленный вид
        value_integer = value / discharge_price;
        //ставим маску для минуса(самый левый бит)
        value_integer = mask | value_integer;
    }
    //возвращаем зашифрованное значение
    //qDebug() << value_integer;
    return value_integer;
}

void setName(Data_block &data, QString text)
{
    std::array<char, 6> name_copy = convertToWindows1251Name(text);

    for(int i = 0; i < text.size(); i++)
    {
        data.name[i] = name_copy[i];
    }

    for(int i = text.size(); i < 6; i++)
    {
        data.name[i] = '\0';
    }
}

void setDescription(Data_block &data, QString text)
{
    std::array<char, 32> description_copy = convertToWindows1251Description(text);

    for(int i = 0; i < text.size(); i++)
    {
        data.description[i] = description_copy[i];
    }

    for(int i = text.size(); i < 32; i++)
    {
        data.description[i] = '\0';
    }
}

uint32_t getClass(const Data_block &data)
{
    return *reinterpret_cast<const uint32_t*>(&data.data[4092]);
}

std::array<char, 6> convertToWindows1251Name(const QString &str)
{
    // Инициализация массива нулями
    std::array<char, 6> result = {0};

    // Открытие конвертера iconv
    iconv_t cd = iconv_open("CP1251", "UTF-8");
    if (cd == (iconv_t)(-1)) {
        qDebug() << "Не удалось открыть iconv";
        return result;
    }

    // Преобразование QString в QByteArray в кодировке UTF-8
    QByteArray utf8Bytes = str.toUtf8();
    const char *inBuf = utf8Bytes.constData();
    size_t inBytesLeft = utf8Bytes.size();

    // Настройка буфера вывода
    char *outBuf = result.data();
    size_t outBytesLeft = result.size();

    // Преобразование строки
    while (inBytesLeft > 0 && outBytesLeft > 0) {
        size_t res = iconv(cd, const_cast<char**>(&inBuf), &inBytesLeft, &outBuf, &outBytesLeft);
        if (res == (size_t)-1) {
            qDebug() << "Ошибка преобразования iconv: " << strerror(errno);
            break;
        }
    }

    // Закрытие конвертера iconv
    iconv_close(cd);

    return result;
}

std::array<char, 32> convertToWindows1251Description(const QString &str)
{
    // Инициализация массива нулями
    std::array<char, 32> result = {0};

    // Открытие конвертера iconv
    iconv_t cd = iconv_open("CP1251", "UTF-8");
    if (cd == (iconv_t)(-1)) {
        qDebug() << "Не удалось открыть iconv";
        return result;
    }

    // Преобразование QString в QByteArray в кодировке UTF-8
    QByteArray utf8Bytes = str.toUtf8();
    const char *inBuf = utf8Bytes.constData();
    size_t inBytesLeft = utf8Bytes.size();

    // Настройка буфера вывода
    char *outBuf = result.data();
    size_t outBytesLeft = result.size();

    // Преобразование строки
    while (inBytesLeft > 0 && outBytesLeft > 0) {
        size_t res = iconv(cd, const_cast<char**>(&inBuf), &inBytesLeft, &outBuf, &outBytesLeft);
        if (res == (size_t)-1) {
            qDebug() << "Ошибка преобразования iconv: " << strerror(errno);
            break;
        }
    }

    // Закрытие конвертера iconv
    iconv_close(cd);

    return result;
}

QByteArray serialize(Data_block &data, uint package_number)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    //заполняем заголовок пакета(6 байт)
    serialize_heading(data, stream);
    //заполняем атрибуты операции(66 байт)
    serialize_attribut(data, stream, buffer, package_number);
    //1024 байт из массива data + 4 байта на параметры
    serialize_data(data, stream, package_number);

    return buffer;
}

void serialize_heading(Data_block &data, QDataStream &stream)
{
    //заполняем заголовок пакета(6 байт)
    //идентификатор сообщения 12 в 16-ой системе
    uint8_t value_ID = 0x12;
    stream << value_ID;

    //длина сообщения 4096/4 = 1024 + 75 байт, 4 пакета по 1099 байт
    uint16_t value_length = 1099;
    stream << value_length;

    //код операции
    uint8_t value_code = 0x02;
    stream << value_code;

    //признак источника
    uint16_t value_source = 0x00;
    stream << value_source;
}

void serialize_attribut(Data_block &data, QDataStream &stream, QByteArray &buffer, uint package_number)
{
    if(package_number > 1)
    {
        //заполняем атрибуты операции(66 байт) все пакеты кроме первого обнуляют эту часть
        const uint8_t value_input = 0;
        for(int i = 0; i < 66; i++)
        {
            stream << value_input;
        }
        //выходим, если данный пакет не первый
        return;
    }

    //заполняем атрибуты операции(66 байт)
    //признак ввода
    uint32_t value_input = 0x10;
    stream << value_input;

    //признак достоверности параметров 1111 = 15
    uint32_t value_reliability = 15;
    stream << value_reliability;

    //широта
    stream << data.latitude;
    //долгота
    stream << data.longitude;
    //высота
    stream << data.altitude;

    //описание объекта
    for(char c : data.description)
    {
        stream << c;
    }
    //имя объекта
    for(char c : data.name)
    {
        stream << c;
    }

    //курс
    stream << data.course;

    //присваиваем указатель на начло buffer, чтобы получить доступ к уже записанным данным
    const char *array_attribute = buffer.data();
    //массив с атрибутами без CRC16
    unsigned char array_attribute_copy[64] = {0};

    //начало участка для вычисления CRC16
    int start_index = 6;
    //размер участка памяти
    int length = 64;

    //копируем необходимые данные из buffer в массив
    std::memcpy(array_attribute_copy, array_attribute + start_index, length);

    uint16_t value_unique = crc16(array_attribute_copy, length);
    //уникальный идентификатор CRC16
    stream << value_unique;
}

void serialize_data(Data_block &data, QDataStream &stream, uint package_number)
{
    //1024 байт из массива data + 4 байта на параметры
    uint32_t value_parameter = 0;
    //общее количество фрагментов информации
    uint value_N = 4;
    value_parameter = value_N;
    value_parameter = value_parameter << 8;
    //порядковый номер фрагмента информации
    uint value_n = package_number;
    value_parameter += value_n;
    value_parameter = value_parameter << 16;
    //количество байтов фрагмента информации
    uint16_t value_data_size = 1024;
    value_parameter += value_data_size;
    //параметры фрагмента информации
    stream << value_parameter;

    //фрагмент информации 1024 * 4 байта = 4096 байта, нам нужно передать 1024 байт
    unsigned char value_inform[1024];

    //индекс, с которого мы заполняем пакет
    int start_index = (package_number - 1) * 1024;
    //индекс конца пакета
    int last_index = 1024 * package_number;

    for (int i = 0, j = start_index; i < 1024 && j < last_index; ++i, ++j)
    {
        //копируем данные для дальнейшей записи в buffer
        value_inform[i] = data.data[j];
        //записываем данные в buffer
        stream << value_inform[i];
    }
}

uint16_t crc16(unsigned char *data, uint lenght)
{
    int j;
    uint16_t reg_crc = 0xFFFF;
    while (lenght--)
    {
        reg_crc ^= *data++;
        for(j = 0; j < 8; j++)
        {
            if(reg_crc & 0x01)
            {
                reg_crc = (reg_crc >> 1) ^ 0xA001;
            }
            else
            {
                reg_crc = reg_crc >> 1;
            }
        }
    }
    return reg_crc;
}
