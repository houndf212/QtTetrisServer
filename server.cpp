#include "server.h"

const QByteArray Server::heart_beat = "heart_beat";

Server::Server(quint16 port, QObject *parent)
    :QObject(parent)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Server::onTimeout);
    timer->start(ktime_check);

    server_socket = new QTcpServer(this);
    connect(server_socket, &QTcpServer::newConnection, this, &Server::onClientComing);
    bool b = server_socket->listen(QHostAddress::Any, port);
    if (!b)
    {
        qDebug() << "listen failed!";
        qApp->exit(-1);
    }
    qDebug() << "listen at: "<<server_socket->serverAddress()<<":"<<server_socket->serverPort();
    //    qDebug() << server_socket->serverAddress();
}

void Server::onTimeout()
{
    qDebug() << "=================";
    qDebug() << "server children size: "<<server_socket->children().size();
//    for(QObject* o : server_socket->children())
//    {
//        qDebug() << o;
//    }
    qDebug() << "check client";
    qDebug() << "delete size: "<< delete_client.size();
    for (QTcpSocket* s : delete_client)
    {
        s->disconnect(this);
        s->deleteLater();
    }
    delete_client.clear();

    getOutTimeoutSocket();
    qDebug() << "ask client size: "<<delete_client.size();
    for (QTcpSocket* s : delete_client)
    {
        s->write(heart_beat);
    }
    qDebug() << "=================";
}

void Server::getOutTimeoutSocket()
{
    QDateTime line = QDateTime::currentDateTime().addMSecs(-ktime_check);
    typedef QMap<QTcpSocket*, QDateTime>::const_iterator Iter;
    for (Iter it=client_map.cbegin(); it!=client_map.cend(); ++it)
    {
        if (it.value()<line)
            delete_client.insert(it.key());
    }
    for(QTcpSocket* s : delete_client)
    {
        client_map.remove(s);
    }
}

void Server::onClientComing()
{
    while(server_socket->hasPendingConnections())
    {
        QTcpSocket *s = server_socket->nextPendingConnection();
        Q_ASSERT(s!=nullptr);
        qDebug() << "incoming: "<<s<<" "<< s->peerAddress()<<":"<<s->peerPort();
        client_map.insert(s, QDateTime::currentDateTime());
        connect(s, &QTcpSocket::disconnected, this, &Server::onClientClose);
        connect(s, &QTcpSocket::readyRead, this, &Server::onDataReady);
    }
}

void Server::onClientClose()
{
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "remove: " <<s;
    client_map.remove(s);
    delete_client.remove(s);
    s->deleteLater();
}

void Server::onDataReady()
{
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    client_map[s] = QDateTime::currentDateTime();
    delete_client.remove(s);
    QByteArray buffer = s->readAll();
    qDebug() <<s <<" " <<s->peerAddress()<<":"<<s->peerPort();
    qDebug() <<buffer;

    if (buffer == heart_beat)
        return;

    for (QTcpSocket* soc : client_map.keys())
    {
        if (soc!=s)
        { soc->write(buffer); }
    }
}
