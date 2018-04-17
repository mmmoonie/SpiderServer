#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QProcess>
#include <QLocalServer>
#include <QLocalSocket>

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit SocketHandler(QObject *parent = 0, qintptr socketDescriptor = 0);
    void on_tcpSocket_readyRead();
    void on_tcpSocket_disconnected();
    void on_tcpSocket_connected();
    void on_localServer_newConnection();
    void on_localSocket_readyRead();
    void on_localSocket_disconnected();

signals:
    void error(const QString &errorMsg);
    void info(const QString &infoMsg);

public slots:
    void start();

private:
    qintptr socketDescriptor;
    QTcpSocket * tcpSocket = 0;
    QProcess * process = 0;
    QLocalServer * localServer = 0;
    QLocalSocket * localSocket = 0;
    void writeToTcpClient(const QJsonObject &json);
};

#endif // SOCKETHANDLER_H
