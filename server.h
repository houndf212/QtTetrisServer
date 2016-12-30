#ifndef SERVER_H
#define SERVER_H
#include <QtCore>
#include <QtNetwork>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(quint16 port, QObject *parent=0);
private:
    Q_SLOT void onTimeout();
    void getOutTimeoutSocket();

    Q_SLOT void onClientComing();
    Q_SLOT void onClientClose();
    Q_SLOT void onDataReady();
private:
    QTimer *timer;
    QTcpServer *server_socket;
    QMap<QTcpSocket*, QDateTime> client_map;
    QSet<QTcpSocket*> delete_client;
    static constexpr int ktime_check = 10*1000;
    static const QByteArray heart_beat;
};

#endif // SERVER_H
