#include "NetSocket.h"
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDebug>
#include <iostream>
#include <QThread>


#define SENDER

const QString   SENDER_ADDR = "127.0.0.1";
const int       SENDER_PORT = 1236;

const QString RECIEVER_ADDR = "127.0.0.1";
const int     RECIEVER_PORT = 1235;


NetSocket::NetSocket()
    : m_sock(new QUdpSocket(this))
{
    connect(m_sock, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &NetSocket::error);

#ifdef SENDER
    if (m_sock->bind(QHostAddress(SENDER_ADDR), SENDER_PORT ) > 0 )
        ;
    else
        qDebug() << "Bind failed";
#else
    if (m_sock->bind(QHostAddress(RECIEVER_ADDR), RECIEVER_PORT ) > 0 )
        ;
    else
        qDebug() << "Bind failed";
#endif

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readyRead()));
     //connect(m_sock, &QUdpSocket::readyRead, this, &NetSocket::onSockReadyRead);

}

NetSocket::~NetSocket()
{
    delete m_sock;
}

void NetSocket::send()
{
    #ifdef SENDER
    while(true)
    {
        static int iter = 0;
        QThread::msleep(50);
        QByteArray Data;
        Data.append("Hello from UDP " + QString::number(iter));
        //Data.append("Hello from UDP");
        //m_sock->writeDatagram(Data, QHostAddress("192.9.206.60"), 1233);
        m_sock->writeDatagram(Data, QHostAddress(RECIEVER_ADDR), RECIEVER_PORT);
        iter++;
        qDebug() <<"mail sended............";
    }
    #endif
}

void NetSocket::onSockConnected()
{
    qDebug() << "Connected";
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

    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
    qDebug() << "-----------------------------";
}

void NetSocket::onSockDisconnected()
{
    qDebug() << "Disconnected";
}

void NetSocket::error()
{
    qDebug() << "error";
}
