#include "NetSocket.h"

#include <QDebug>
#include <iostream>
#include <QThread>
#include <mutex>
#include <queue>
#include "Settings/Settings.h"
#include <pulse/pulseaudio.h>
extern uint8_t* mainBuff;
extern std::mutex mutexMainBuff;
extern uint32_t lenMainBuff;
extern std::queue<uint8_t> queueBuff;
extern pa_threaded_mainloop *mloop;
extern pa_channel_map* map;
extern pa_cvolume* vol;
extern pa_context *ctx;
extern pa_stream *stream;
extern pa_stream *streamOut;


NetSocket::NetSocket(const QString &addr, int port)
    : m_sock(new QUdpSocket(this))
    , m_port(port)
    , m_addr(addr)
    , m_recv()
{
    if (m_sock->bind(QHostAddress(m_addr), m_port ) > 0 )
        qDebug() << "Bind successfull";
    else
        qDebug() << "Bind failed";

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

int NetSocket::receive(char *data, int maxlen)
{
    qint64 res = m_sock->readDatagram(data, maxlen);
    return res;
}

void NetSocket::setReceiveMethod(const ReceiveMethod &method)
{
    m_recv = method;
    connect(m_sock, &QUdpSocket::readyRead, this, m_recv);
}

void NetSocket::readyRead()
{
//#if 1
//    static const int16_t fragsize = pulse::Settings::instance().value(pulse::Settings::bufferFragSize).toInt();

//    QByteArray buffer(fragsize, 0);
//    size_t len  = m_sock->readDatagram(buffer.data(), fragsize);
//    if (len <= 0)
//        return;

//    size_t requested_bytes = pa_stream_writable_size(streamOut);
//    if (len > requested_bytes)  len = requested_bytes;

//    uint8_t* b;
//    pa_stream_begin_write(streamOut, reinterpret_cast<void**>(&b), &len);

//    for (int i = 0; i < len; ++i) b[i] = (buffer.data())[i];

//    pa_stream_write(streamOut, b, len, nullptr, 0, PA_SEEK_RELATIVE);
//#endif

//#if 0
//        QByteArray buffer;
//        buffer.resize(1024);

//        int len  = m_sock->readDatagram(buffer.data(), buffer.size());

//        if (len < 0)
//            return;

//        std::lock_guard<std::mutex> lock(mutexMainBuff);
//        for (int i = 0; i < len; ++i) queueBuff.push((buffer.data())[i]);
//#endif
}

void NetSocket::error(QAbstractSocket::SocketError err )
{
    qDebug() << "NetSocket error: " << err;
}
