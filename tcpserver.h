#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);

signals:
    void info(const QString &msg);

public slots:
    void on_SocketHandler_info(const QString &msg);

protected:
    void incomingConnection(qintptr socketDescriptor);
};

#endif // TCPSERVER_H
