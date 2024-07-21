#include "client.h"

#define PORT 49101

Client::Client(QObject *parent)
    : QObject(parent)
    , mClient(new QUdpSocket(this))
{}

void Client::send(const QByteArray &buffer)
{
    mClient->writeDatagram(buffer, QHostAddress::LocalHost, PORT);
}
