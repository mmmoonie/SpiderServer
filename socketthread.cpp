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
    // init tcpSocket
    tcpSocket = new QTcpSocket();
    if(!tcpSocket->setSocketDescriptor(this->socketDescriptor))
    {
        emit info(tcpSocket->errorString());
        return;
    }
    connect(tcpSocket, &QTcpSocket::readyRead, this, &SocketThread::on_tcpSocket_readyRead, Qt::DirectConnection);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &SocketThread::on_tcpSocket_disconnected);

    // init localServer
    // listen on uuid
    localServer = new QLocalServer();
    connect(localServer, &QLocalServer::newConnection, this, &SocketThread::on_localServer_newConnection);
    QString uuid = QUuid::createUuid().toString();
    if(!localServer->listen(uuid))
    {
        QString errorMsg = QString("local server listen on %1 failed").arg(uuid);
        emit info(errorMsg);
        QJsonObject errorJson;
        errorJson.insert("code", 500);
        errorJson.insert("desc", errorMsg);
        errorJson.insert("data", QJsonValue::Null);
        writeToTcpClient(errorJson);
        tcpSocket->close();
        return;
    }
    emit info("localServer is listening on " + uuid);

    // search config file
    // get the WebVewSpider.exe path and start the process
    QString configFilePath = ConfigUtil::APP_SETTINGS_PATH();
    if(!configFilePath.isNull() && !configFilePath.isEmpty())
    {
        emit info(QString("useing config file : ").append(configFilePath));
        QSettings * defaultSettings = new QSettings(configFilePath, QSettings::IniFormat);
        defaultSettings->beginGroup("default");
        QString spiderExePath = defaultSettings->value("spiderExePath").toString();
        QFileInfo fileInfo(spiderExePath);
        if(!fileInfo.exists())
        {
            emit info(spiderExePath + " not exists");
            QJsonObject errorJson;
            errorJson.insert("code", 500);
            errorJson.insert("desc", spiderExePath + " not exists");
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
        emit info(spiderExePath + " started");
        localServer->waitForNewConnection(10000);
    }
    else
    {
        emit info("config file not found");
        return;
    }

    exec();
}

void SocketThread::on_tcpSocket_readyRead()
{
    QByteArray data = tcpSocket->readAll();
    emit info(QString::number(socketDescriptor) + " tcpsocket data in: " + data);
    localSocket->write(data);
}

void SocketThread::on_tcpSocket_disconnected()
{
    emit info(QString::number(socketDescriptor) + " tcpsocket disconnected");
    if(process && process->state() != QProcess::NotRunning)
    {
        process->kill();
        emit info(process->program() + " killed");
        process->deleteLater();
    }
    tcpSocket->deleteLater();
    exit(0);
}

void SocketThread::on_localServer_newConnection()
{
    localSocket = localServer->nextPendingConnection();
    emit info(localServer->serverName() + " has new connection");
    connect(localSocket, &QLocalSocket::readyRead, this, &SocketThread::on_localSocket_readyRead, Qt::DirectConnection);
    connect(localSocket, &QLocalSocket::disconnected, this, &SocketThread::on_localSocket_disconnected);
}

void SocketThread::on_localSocket_readyRead()
{
    QByteArray data = localSocket->readAll();
    emit info(QString(data));
    int count = data.count();
    tcpSocket->write(data);
    tcpSocket->write("\r\n");
    if(count != 4096)
    {
        tcpSocket->write("boundary-----------");
    }
    tcpSocket->write("\r\n");
}

void SocketThread::on_localSocket_disconnected()
{
    emit(localSocket->socketDescriptor() + " disconnected");
    localSocket->deleteLater();
    localServer->deleteLater();
}


void SocketThread::writeToTcpClient(const QJsonObject &json)
{
    QJsonDocument doc;
    doc.setObject(json);
    tcpSocket->write(doc.toJson());
}
