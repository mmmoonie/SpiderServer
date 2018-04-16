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

public slots:

protected:
    void incomingConnection(qintptr socketDescriptor);
};

#endif // TCPSERVER_H
