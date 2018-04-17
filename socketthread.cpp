#include "socketthread.h"
#include "configutil.h"
#include <QUuid>
#include <QJsonDocument>
#include <QSettings>
#include <QFileInfo>

SocketThread::SocketThread(qintptr ID, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;
}

void SocketThread::run()
{
    tcpSocket = new QTcpSocket();

    if(!tcpSocket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(tcpSocket->error());
        return;
    }
    connect(tcpSocket, &QTcpSocket::readyRead, this, &SocketThread::on_tcpSocket_readyRead, Qt::DirectConnection);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &SocketThread::on_tcpSocket_disconnected);

    localServer = new QLocalServer();
    connect(localServer, &QLocalServer::newConnection, this, &SocketThread::on_localServer_newConnection);

    QString uuid = QUuid::createUuid().toString();
    if(!localServer->listen(uuid))
    {
        QString errorMsg = QString("local server listen on %1 failed").arg(uuid);
        QJsonObject errorJson;
        errorJson.insert("code", 500);
        errorJson.insert("desc", errorMsg);
        errorJson.insert("data", QJsonValue::Null);
        writeToTcpClient(errorJson);
        tcpSocket->close();
        return;
    }

    QString configFilePath = ConfigUtil::APP_SETTINGS_PATH();
    QSettings * defaultSettings = new QSettings(configFilePath, QSettings::IniFormat);
    defaultSettings->beginGroup("default");
    QString spiderExePath = defaultSettings->value("spiderExePath").toString();
    QFileInfo fileInfo(spiderExePath);
    if(!fileInfo.exists())
    {
        QJsonObject errorJson;
        errorJson.insert("code", 500);
        errorJson.insert("desc", QString(spiderExePath.append(" not exists")));
        errorJson.insert("data", QJsonValue::Null);
        writeToTcpClient(errorJson);
        tcpSocket->close();
        return;
    }
    QStringList arguments;
    arguments << uuid;
    process = new QProcess();
    process->start(spiderExePath, arguments);
    process->waitForStarted();
    localServer->waitForNewConnection(10000);
    exec();
}

void SocketThread::on_tcpSocket_readyRead()
{
    QByteArray Data = tcpSocket->readAll();
    qDebug() << socketDescriptor << " Data in: " << Data;

    localSocket->write(Data);
    localSocket->flush();
    localSocket->waitForReadyRead();
}

void SocketThread::on_tcpSocket_disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    if(process && process->state() != QProcess::NotRunning)
    {
        process->kill();
        process->deleteLater();
    }
    tcpSocket->deleteLater();
    exit(0);
}

void SocketThread::on_localServer_newConnection()
{
    localSocket = localServer->nextPendingConnection();
    connect(localSocket, &QLocalSocket::readyRead, this, &SocketThread::on_localSocket_readyRead, Qt::DirectConnection);
    connect(localSocket, &QLocalSocket::disconnected, this, &SocketThread::on_localSocket_disconnected);
}

void SocketThread::on_localSocket_readyRead()
{
    QByteArray data = localSocket->readAll();
    qDebug() << "localSocket Data in: " << data;
    tcpSocket->write(data);
    tcpSocket->write("\r\n");
    tcpSocket->write("\r\n");
    tcpSocket->flush();
    tcpSocket->waitForBytesWritten();
}

void SocketThread::on_localSocket_disconnected()
{
    localSocket->deleteLater();
    localServer->deleteLater();
}


void SocketThread::writeToTcpClient(const QJsonObject &json)
{
    QJsonDocument doc;
    doc.setObject(json);
    tcpSocket->write(doc.toJson());
    tcpSocket->flush();
}
