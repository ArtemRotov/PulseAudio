#include "NetSocket.h"

#include <QDebug>
#include <iostream>
#include <QThread>
#include <mutex>
#include <queue>

#include <pulse/pulseaudio.h>
extern uint8_t* mainBuff;
extern std::mutex mutexMainBuff;
extern uint32_t lenMainBuff;
extern std::queue<uint8_t> queueBuff;
extern pa_threaded_mainloop *mloop;
extern pa_channel_map* map;
extern pa_cvolume* vol;

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
    static int i = 0;
    i++;
    if (i == 1000)
    {
        pa_cvolume_reset(vol,2);
        qDebug() << "reset";
    }
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
}

void NetSocket::error(QAbstractSocket::SocketError err )
{
    qDebug() << "error " << err;
}
