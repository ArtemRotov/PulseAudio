#include "NetSocket.h"

#include <QDebug>
#include <iostream>
#include <QThread>
#include <mutex>
#include <queue>


extern uint8_t* mainBuff;
extern std::mutex mutexMainBuff;
extern uint32_t lenMainBuff;
extern std::queue<uint8_t> queueBuff;

NetSocket::NetSocket(const QString &addr, int port)
    : m_sock(new QUdpSocket(this))
    , m_port(port)
    , m_addr(addr)
{
    if (m_sock->bind(QHostAddress(m_addr), m_port ) > 0 )
        qDebug() << "Bind successfull";
    else
        qDebug() << "Bind failed";
    connect(m_sock, &QUdpSocket::readyRead, this, &NetSocket::readyRead);
    connect(m_sock, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &NetSocket::error);
    qDebug() << m_sock->state();
}

NetSocket::~NetSocket()
{
    delete m_sock;
}

qint64 NetSocket::send(void* data, qint64 len, const QString &addr, int port)
{
    auto res = m_sock->writeDatagram((char*)data, len, QHostAddress(addr),port);
    return res;
}

qint64 NetSocket::read(char *data, qint64 maxlen)
{
    qint64 res = m_sock->readDatagram(data, maxlen);
    //qDebug() << "readed " << res;
    return res;

}

void NetSocket::readyRead()
{
    QByteArray buffer;
    buffer.resize(1024);

    int len  = m_sock->readDatagram(buffer.data(), buffer.size());
    //qDebug() << "Readed " << len << " bytes (and removed old)";
    if (len < 0)
        return;

    std::lock_guard<std::mutex> lock(mutexMainBuff);
    for (int i = 0; i < len; ++i)
    {
        queueBuff.push((buffer.data())[i]);
    }

//    //qDebug() << "readyRead socket";
//    QByteArray buffer;
//    buffer.resize(1024);

//    int len  = m_sock->readDatagram(buffer.data(), buffer.size());
//    qDebug() << "Readed " << len << " bytes (and removed old)";
//    if (len < 0)
//        return;

//    std::lock_guard<std::mutex> lock(mutexMainBuff);
//    lenMainBuff = len;
//    delete [] mainBuff;
//    mainBuff = new uint8_t[lenMainBuff];
//    memcpy(mainBuff, buffer.data(), lenMainBuff);
}

void NetSocket::error(QAbstractSocket::SocketError err )
{
    qDebug() << "error " << err;
}
