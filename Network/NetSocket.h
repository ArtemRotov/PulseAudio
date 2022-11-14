#include <QObject>
#include <QUdpSocket>


class NetSocket : public QObject
{
    Q_OBJECT

public:
    NetSocket(const QString &addr, int port);
    ~NetSocket();

    qint64 send(char* data, qint64 len, const QString &addr, int port);
    qint64 read(char* data, qint64 maxlen);

private slots:
    void onSockConnected();
    void readyRead();
    void onSockDisconnected();
    void error();

private:
    QUdpSocket* m_sock;
    int         m_port;
    QString     m_addr;
};


