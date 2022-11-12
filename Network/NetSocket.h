#include <QObject>


class QUdpSocket;

class NetSocket : public QObject
{
    Q_OBJECT

public:
    NetSocket();
    ~NetSocket();

    void send();

private slots:
    void onSockConnected();
    void readyRead();
    void onSockDisconnected();
    void error();

private:
    QUdpSocket* m_sock;

};


