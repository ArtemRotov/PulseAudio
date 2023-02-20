#include <QObject>
#include <QUdpSocket>

class NetSocket : public QObject
{
    Q_OBJECT
public:
    using ReceiveMethod = std::function<void ()>;

    NetSocket(const QString &addr, int port);
    ~NetSocket();

    int receive(char* data, int maxlen);
    qint64 send(void* data, qint64 len, const QString &addr, int port);

    void setReceiveMethod(const ReceiveMethod &method);

private slots:
    void error(QAbstractSocket::SocketError err );

private:
    QUdpSocket*     m_sock;
    int             m_port;
    QString         m_addr;

    ReceiveMethod   m_recv;
};


