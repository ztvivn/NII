#include "data_block.h"
#include <iconv.h>
#include <QString>
#include <array>
#include <iostream>
#include <cstring>

#define MASK_LATITUDE 16384//14 бит для минуса широты
#define MASK_LONGITUDE 32768//15 бит для минуса долготы

Data_block::Data_block() {}

Data_block deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    Data_block objectData;

    uint8_t value_ID;//идентификатор сообщ
    stream >> value_ID;

    uint16_t value_length;//длина сообщ
    stream >> value_length;

    uint8_t value_code;//код операции
    stream >> value_code;

    uint16_t value_source;//признак источника
    stream >> value_source;

    uint32_t value_input;//признак ввода
    stream >> value_input;

    uint32_t value_reliability;//признак достоверности
    stream >> value_reliability;

    uint32_t latitude;//широта
    stream >> latitude;
    uint32_t longitude;//долгота
    stream >> longitude;
    uint32_t altitude;//высота
    stream >> altitude;
    std::array<char, 32> description;//описание объекта
    for (char &c : description)
    {
        stream >> c;
    }
    std::array<char, 8> name;//имя объекта
    for (char &c : name)
    {
        stream >> c;
    }
    uint32_t course;//путевой угол
    stream >> course;

    uint16_t value_unique;//уникальный идентификатор crc16
    stream >> value_unique;

    uint8_t value_N;//общее количество информации
    stream >> value_N;
    uint8_t value_n;//порядковый номер фрагмента информации
    stream >> value_n;
    objectData.value_n = value_n;
    uint16_t data_size;//количество байтов фрагмента информации
    stream >> data_size;

    unsigned char value_inform[1024];
    for(int i = 0; i < 1024; ++i)//фрагмент информации
    {
        stream >> value_inform[i];
    }

    if(value_n > 1)
    {
        for (int i = 0, j = (value_n - 1) * 1024; i < 1024 && j < 1024 * value_n; ++i, ++j)//записываем в массив данные из пакета
        {
            objectData.data[j] = value_inform[i];
        }

        return objectData;
    }

    for(int i = 0; i < 1024; ++i)//заполняем данные для массива из первого пакета
    {
        objectData.data[i] = value_inform[i];
    }

    if(latitude > 10000)//убираем маску, которая показывает минус
    {
        latitude = latitude ^ MASK_LATITUDE;
        double latitude_copy = latitude;
        objectData.latitude = (latitude_copy * (-1)) / 100;
    }else
    {
        double latitude_copy = latitude;
        objectData.latitude = latitude_copy / 100;
    }
    if(longitude > 20000)
    {
        longitude = longitude ^ MASK_LONGITUDE;
        double longitude_copy = longitude;
        objectData.longitude = (longitude_copy * (-1)) / 100;
    }
    else
    {
        double longitude_copy = longitude;
        objectData.longitude = longitude_copy / 100;
    }
    double altitude_copy = altitude;
    objectData.altitude = altitude_copy / 100;
    double course_copy = course;
    objectData.course = course_copy / 100;
    objectData.description = description;
    objectData.name = name;


    return objectData;
}

QString Data_block::getData()
{
    uint32_t value;
    QString value_str;

    value = *reinterpret_cast<uint32_t*>(&data[4092]);
    value_str = QString::number(value);//переводим значение в строку, чтобы вывести ее на экран

    return value_str;
}

QString Data_block::getName()
{
    QString name_str;
    name_str = convertFromWindows1251Name(this->name);

    return name_str;
}

QString Data_block::getDescription()
{
    QString description_str;
    description_str = convertFromWindows1251Description(this->description);

    return description_str;
}

QString Data_block::convertFromWindows1251Name(const std::array<char, 8> &inputArray)
{
    // Открытие конвертера iconv
    iconv_t cd = iconv_open("UTF-8", "CP1251");
    if (cd == (iconv_t)(-1)) {
        std::cerr << "Не удалось открыть iconv" << std::endl;
        return QString();
    }

    // Настройка буфера ввода
    const char *inBuf = inputArray.data();
    size_t inBytesLeft = inputArray.size();

    // Настройка буфера вывода
    size_t outBufSize = inBytesLeft * 2; // Достаточно большой буфер для результата
    char *outBuf = new char[outBufSize];
    char *outBufStart = outBuf;
    size_t outBytesLeft = outBufSize;

    // Преобразование строки
    while (inBytesLeft > 0 && outBytesLeft > 0) {
        size_t res = iconv(cd, const_cast<char**>(&inBuf), &inBytesLeft, &outBuf, &outBytesLeft);
        if (res == (size_t)-1) {
            std::cerr << "Ошибка преобразования iconv: " << strerror(errno) << std::endl;
            break;
        }
    }

    // Создание QString из результирующего буфера UTF-8
    QString result = QString::fromUtf8(outBufStart, outBufSize - outBytesLeft);

    // Освобождение памяти и закрытие конвертера iconv
    delete[] outBufStart;
    iconv_close(cd);

    return result;
}

QString Data_block::convertFromWindows1251Description(const std::array<char, 32> &inputArray)
{
    // Открытие конвертера iconv
    iconv_t cd = iconv_open("UTF-8", "CP1251");
    if (cd == (iconv_t)(-1)) {
        std::cerr << "Не удалось открыть iconv" << std::endl;
        return QString();
    }

    // Настройка буфера ввода
    const char *inBuf = inputArray.data();
    size_t inBytesLeft = inputArray.size();

    // Настройка буфера вывода
    size_t outBufSize = inBytesLeft * 2; // Достаточно большой буфер для результата
    char *outBuf = new char[outBufSize];
    char *outBufStart = outBuf;
    size_t outBytesLeft = outBufSize;

    // Преобразование строки
    while (inBytesLeft > 0 && outBytesLeft > 0) {
        size_t res = iconv(cd, const_cast<char**>(&inBuf), &inBytesLeft, &outBuf, &outBytesLeft);
        if (res == (size_t)-1) {
            std::cerr << "Ошибка преобразования iconv: " << strerror(errno) << std::endl;
            break;
        }
    }

    // Создание QString из результирующего буфера UTF-8
    QString result = QString::fromUtf8(outBufStart, outBufSize - outBytesLeft);

    // Освобождение памяти и закрытие конвертера iconv
    delete[] outBufStart;
    iconv_close(cd);

    return result;
}
