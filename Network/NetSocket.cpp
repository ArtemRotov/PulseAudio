#include "NetSocket.h"
#include <QDebug>

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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    connect(m_sock, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &NetSocket::error);
#else
    connect(m_sock, &QAbstractSocket::errorOccurred, this, &NetSocket::error);
#endif
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

void NetSocket::error(QAbstractSocket::SocketError err )
{
    qDebug() << "NetSocket error: " << err;
}
