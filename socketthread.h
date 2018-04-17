#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QProcess>
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonObject>

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(qintptr ID, QObject *parent = 0);

    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void on_tcpSocket_readyRead();
    void on_tcpSocket_disconnected();
    void on_localServer_newConnection();
    void on_localSocket_readyRead();
    void on_localSocket_disconnected();


private:
    QTcpSocket * tcpSocket = 0;
    qintptr socketDescriptor;
    QProcess * process = 0;
    QLocalServer * localServer = 0;
    QLocalSocket * localSocket = 0;
    void writeToTcpClient(const QJsonObject &json);
};

#endif // SOCKETTHREAD_H
