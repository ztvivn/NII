#include "deserialization.h"

#include <iconv.h>
#include <iostream>

//14 бит для минуса широты
#define MASK_LATITUDE 16384
//15 бит для минуса долготы
#define MASK_LONGITUDE 32768


Data_block deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    Data_block objectData;
    //идентификатор сообщения
    uint8_t value_ID;
    stream >> value_ID;
    //длина сообщ
    uint16_t value_length;
    stream >> value_length;
    //код операции
    uint8_t value_code;
    stream >> value_code;
    //признак источника
    uint16_t value_source;
    stream >> value_source;
    //признак ввода
    uint32_t value_input;
    stream >> value_input;
    //признак достоверности
    uint32_t value_reliability;
    stream >> value_reliability;
    //широта
    uint32_t latitude;
    stream >> latitude;
    //долгота
    uint32_t longitude;
    stream >> longitude;
    //высота
    uint32_t altitude;
    stream >> altitude;
    //описание объекта
    std::array<char, 32> description;
    for (char &c : description)
    {
        stream >> c;
    }
    //имя объекта
    std::array<char, 8> name;
    for (char &c : name)
    {
        stream >> c;
    }
    //путевой угол
    uint32_t course;
    stream >> course;

    //уникальный идентификатор crc16
    uint16_t value_unique;
    stream >> value_unique;

    //общее количество информации
    uint8_t value_N;
    stream >> value_N;
    //порядковый номер фрагмента информации
    uint8_t value_n;
    stream >> value_n;
    objectData.value_n = value_n;
    //количество байтов фрагмента информации
    uint16_t data_size;
    stream >> data_size;

    unsigned char value_inform[1024];
    //фрагмент информации
    for(int i = 0; i < 1024; ++i)
    {
        stream >> value_inform[i];
    }

    if(value_n > 1)
    {
        //записываем в массив данные из пакета
        for (int i = 0, j = (value_n - 1) * 1024; i < 1024 && j < 1024 * value_n; ++i, ++j)
        {
            objectData.data[j] = value_inform[i];
        }

        return objectData;
    }

    //заполняем данные для массива из первого пакета
    for(int i = 0; i < 1024; ++i)
    {
        objectData.data[i] = value_inform[i];
    }

    //убираем маску, которая показывает минус
    if(latitude > 10000)
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

uint32_t getClass(const Data_block &data)
{
    return *reinterpret_cast<const uint32_t*>(&data.data[4092]);
}

QString getName(Data_block data_array)
{
    QString name_str;
    name_str = convertFromWindows1251Name(data_array.name);

    return name_str;
}

QString getDescription(Data_block data_array)
{
    QString description_str;
    description_str = convertFromWindows1251Description(data_array.description);

    return description_str;
}

QString convertFromWindows1251Name(const std::array<char, 8> &inputArray)
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
    size_t outBufSize = inBytesLeft * 2;
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

QString convertFromWindows1251Description(const std::array<char, 32> &inputArray)
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
    size_t outBufSize = inBytesLeft * 2;
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
