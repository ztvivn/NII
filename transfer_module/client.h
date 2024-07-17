#ifndef CLIENT_H
#define CLIENT_H

#include "data_block.h"
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QUdpSocket>
#include <QObject>

class Client : public QObject
{
public:
    Client(QObject *parent = nullptr);
    QByteArray serialize(Data_block &data, uint package_number);//сериализация пакета
    uint16_t crc16(unsigned char *array, uint lenght);//алгоритм crc16
    void send(const QByteArray &buffer);//отправка сообщения

private:
    QUdpSocket *mClient;
};

#endif // CLIENT_H
