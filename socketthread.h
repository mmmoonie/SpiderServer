#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QProcess>
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonObject>

#if QT_VERSION >= 0x050000
    typedef qintptr tSocketDescriptor;
#else
    typedef int tSocketDescriptor;
#endif

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(QObject *parent = 0);
    void run();
    ~SocketThread();

signals:
    void info(const QString infoMsg);

public slots:
    void on_tcpSocket_readyRead();
    void on_tcpSocket_disconnected();
    void on_localServer_newConnection();
    void on_localSocket_readyRead();
    void on_localSocket_disconnected();
    void handleConnection(tSocketDescriptor id);


private:
    QTcpSocket * tcpSocket = 0;
    qintptr socketDescriptor;
    QProcess process;
    QLocalServer localServer;
    QLocalSocket * localSocket = 0;
    void writeToTcpClient(const QJsonObject &json);
};

#endif // SOCKETTHREAD_H
