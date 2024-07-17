#include "client.h"

Client::Client(QObject *parent) : QObject(parent), mClient(new QUdpSocket(this))
{}

void Client::send(const QByteArray &buffer)
{
    mClient->writeDatagram(buffer, QHostAddress::LocalHost, 49101);
}

uint16_t Client::crc16(unsigned char *data, uint lenght)
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

QByteArray Client::serialize(Data_block &data, uint package_number)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    if(package_number > 1 && package_number <= 4)//для формирования пакетов 2-4
    {
        //заполняем заголовок пакета(6 байт)
        uint8_t value_ID = 0x0C;//идентификатор сообщения 12 в 16-ой системе
        stream << value_ID;

        uint16_t value_length = 1099;//длина сообщения 4096/4 = 1024 + 75 байт, 4 пакета по 1099 байт
        stream << value_length;

        uint8_t value_code = 0x02;//код операции
        stream << value_code;

        uint16_t value_source = 0x00;//признак источника
        stream << value_source;

        //заполняем атрибуты операции(66 байт)
        uint32_t value_input = 0;//признак ввода
        stream << value_input;

        uint32_t value_reliability = 0;//признак достоверности параметров
        stream << value_reliability;

        uint32_t latitude = 0;
        stream << latitude;//широта
        uint32_t longitude = 0;
        stream << longitude;//долгота
        uint32_t altitude = 0;
        stream << altitude;//высота

        std::array<char, 32> description;
        description.fill(0);//заполняем нулями
        for(char c : description)//описание объекта
        {
            stream << c;
        }
        std::array<char, 8> name;
        name.fill(0);
        for(char c : name)//имя объекта
        {
            stream << c;
        }

        uint32_t course = 0;
        stream << course;//курс

        uint16_t value_unique = 0;
        stream << value_unique;//уникальный идентификатор CRC16

        //1024 байт из массива data + 4 байта на параметры
        uint32_t value_parameter = 0;
        uint value_N = 4;//общее количество фрагментов информации
        value_parameter = value_N;
        value_parameter = value_parameter << 8;
        uint value_n = package_number;//порядковый номер фрагмента информации
        value_parameter += value_n;
        value_parameter = value_parameter << 16;
        uint16_t value_data_size = 1024;//количество байтов фрагмента информации
        value_parameter += value_data_size;
        stream << value_parameter;//параметры фрагмента информации

        unsigned char value_inform[1024];//фрагмент информации 1024 * 4 байта = 4096 байта, нам нужно передать 1024 байт

        for (int i = 0, j = (package_number - 1) * 1024; i < 1024 && j < 1024 * package_number; ++i, ++j)
        {
            value_inform[i] = data.data[j];
        }
        for(unsigned char c : value_inform)//фрагмент информации
        {
            stream << c;
        }

        return buffer;
    }
    //заполняем заголовок пакета(6 байт)
    uint8_t value_ID = 0x0C;//идентификатор сообщения 12 в 16-ой системе
    stream << value_ID;

    uint16_t value_length = 1099;//длина сообщения 4096/4 = 1024 + 75 байт, 4 пакета по 1099 байт
    stream << value_length;

    uint8_t value_code = 0x02;//код операции
    stream << value_code;

    uint16_t value_source = 0x00;//признак источника
    stream << value_source;

    //заполняем атрибуты операции(66 байт)
    uint32_t value_input = 0x0A;//признак ввода
    stream << value_input;

    uint32_t value_reliability = 15;//признак достоверности параметров 1111 = 15
    stream << value_reliability;

    uint32_t latitude = data.getLatitude();
    stream << latitude;//широта
    uint32_t longitude = data.getLongitude();
    stream << longitude;//долгота
    uint32_t altitude = data.getAltitude();
    stream << altitude;//высота

    std::array<char, 32> description = data.getDescription();
    for(char c : description)//описание объекта
    {
        stream << c;
    }
    std::array<char, 8> name = data.getName();
    for(char c : name)//имя объекта
    {
        stream << c;
    }

    uint32_t course = data.getCourse();
    stream << course;//курс

    unsigned char array_attribute[64] = {0};//массив с атрибутами без CRC16
    size_t offset = 0;

    std::memcpy(array_attribute + offset, &value_input, sizeof(value_input));
    offset += sizeof(value_input);
    std::memcpy(array_attribute + offset, &value_reliability, sizeof(value_reliability));
    offset += sizeof(value_reliability);
    std::memcpy(array_attribute + offset, &latitude, sizeof(latitude));
    offset += sizeof(latitude);
    std::memcpy(array_attribute + offset, &longitude, sizeof(longitude));
    offset += sizeof(longitude);
    std::memcpy(array_attribute + offset, &altitude, sizeof(altitude));
    offset += sizeof(altitude);
    std::memcpy(array_attribute + offset, description.data(), description.size());
    offset += description.size();
    std::memcpy(array_attribute + offset, name.data(), name.size());
    offset += name.size();
    std::memcpy(array_attribute + offset, &course, sizeof(course));
    offset += sizeof(course);

    uint16_t value_unique = crc16(array_attribute, offset);
    stream << value_unique;//уникальный идентификатор CRC16

    //1024 байт из массива data + 4 байта на параметры
    uint32_t value_parameter = 0;
    uint8_t value_N = 4;//общее количество фрагментов информации
    value_parameter = value_N;
    value_parameter = value_parameter << 8;
    uint8_t value_n = 1;//порядковый номер фрагмента информации
    value_parameter += value_n;
    value_parameter = value_parameter << 16;
    uint16_t value_data_size = 1024;//количество байтов фрагмента информации
    value_parameter += value_data_size;
    stream << value_parameter;//параметры фрагмента информации

    unsigned char value_inform[1024];//фрагмент информации 1024 * 4 байта = 4096 байта, нам нужно передать 1024 байт

    for (int i = 0; i < 1024; ++i)
    {
        value_inform[i] = data.data[i];
    }
    for(unsigned char c : value_inform)//фрагмент информации
    {
        stream << c;
    }

    return buffer;
}
