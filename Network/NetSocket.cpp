#include "NetSocket.h"

#include <QDebug>
#include <iostream>
#include <QThread>


NetSocket::NetSocket(const QString &addr, int port)
    : m_sock(new QUdpSocket(this))
    , m_port(port)
    , m_addr(addr)
{
    //connect(m_sock, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &NetSocket::error);

    if (m_sock->bind(QHostAddress(m_addr), m_port ) > 0 )
        qDebug() << "Bind successfull";
    else
        qDebug() << "Bind failed";

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

NetSocket::~NetSocket()
{
    delete m_sock;
}

qint64 NetSocket::send(void* data, qint64 len, const QString &addr, int port)
{
    auto res = m_sock->writeDatagram((char*)data, len, QHostAddress(addr),port);

    //qDebug() << "sended " << res << "bytes to " << addr << ":" << port;

    return res;
}

void NetSocket::onSockConnected()
{
    qDebug() << "Connected";
}

qint64 NetSocket::read(char *data, qint64 maxlen)
{
    qint64 res = m_sock->readDatagram(data, maxlen);
    //qDebug() << "readed " << res;
    return res;

}

void NetSocket::readyRead()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(m_sock->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    m_sock->readDatagram(buffer.data(), buffer.size(),
                         &sender, &senderPort);

//    qDebug() << "Message from: " << sender.toString();
//    qDebug() << "Message port: " << senderPort;
//    qDebug() << "Message: " << buffer;
//    qDebug() << "-----------------------------";
}

void NetSocket::onSockDisconnected()
{
    qDebug() << "Disconnected";
}

void NetSocket::error()
{
    qDebug() << "error";
}
