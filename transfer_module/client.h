#ifndef CLIENT_H
#define CLIENT_H

#include <QUdpSocket>
#include <QByteArray>
#include <QObject>

class Client : public QObject
{
public:
    Client(QObject *parent = nullptr);
    //отправка сообщения
    void send(const QByteArray &buffer);

private:
    QUdpSocket *mClient;
};

#endif // CLIENT_H
