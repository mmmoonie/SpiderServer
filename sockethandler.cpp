#include "sockethandler.h"
#include <QJsonDocument>
#include <QUuid>
#include <QJsonParseError>
#include <QStringList>
#include <QSettings>
#include <QFileInfo>
#include "configutil.h"

SocketHandler::SocketHandler(QObject *parent, qintptr socketDescriptor) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    tcpSocket->setSocketDescriptor(socketDescriptor);
    connect(tcpSocket, &QTcpSocket::connected, this, &SocketHandler::on_tcpSocket_connected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &SocketHandler::on_tcpSocket_readyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &SocketHandler::on_tcpSocket_disconnected);
    localServer = new QLocalServer(this);
    connect(localServer, &QLocalServer::newConnection, this, &SocketHandler::on_localServer_newConnection);

    process = new QProcess;
    QString uuid = QUuid::createUuid().toString();
    if(!localServer->listen(uuid))
    {
        QString errorMsg = QString("local server listen on %1 failed").arg(uuid);
        emit error(errorMsg);
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
        emit error(spiderExePath.append(" not exists"));
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
    qDebug() << spiderExePath << uuid;
    process->start(spiderExePath, arguments);
    process->waitForStarted();
    localServer->waitForNewConnection(10000);
    qDebug() << "process start";
}

void SocketHandler::on_localServer_newConnection()
{
    localSocket = localServer->nextPendingConnection();
//    localSocket->write("124");
//    localSocket->flush();
    qDebug() << "socket descriptor " << localSocket->socketDescriptor();
//    connect(localSocket, &QLocalSocket::readyRead, this, &SocketHandler::on_localSocket_readyRead);
    connect(localSocket, &QLocalSocket::readyRead, [=](){
        QByteArray data = localSocket->readAll();
        qDebug() << data;
        emit info(QString(data));
        qDebug() << tcpSocket->state();
        tcpSocket->write(data);
        tcpSocket->flush();
    });
    connect(localSocket, &QLocalSocket::disconnected, this, &SocketHandler::on_localSocket_disconnected);
}

void SocketHandler::on_localSocket_readyRead()
{
    QByteArray data = localSocket->readAll();
    emit info(QString(data));
    tcpSocket->write(data);
    tcpSocket->flush();
}

void SocketHandler::on_localSocket_disconnected()
{
//    localSocket->close();
//    localServer->close();
//    tcpSocket->close();
}

void SocketHandler::on_tcpSocket_connected()
{

}

void SocketHandler::on_tcpSocket_readyRead()
{
    QByteArray data = tcpSocket->readAll();
    qDebug() << "tcpSocket state: " << tcpSocket->state();
    qDebug() << "tcpSocket data: " << data;
    emit info(QString(data));
    localSocket->write("123");
    if(localSocket->flush())
    {
        qDebug() << "tcpsocket to localsocket success";
    }
    else
    {
        qDebug() << "tcpsocket to localsocket failed";
    }
}

void SocketHandler::writeToTcpClient(const QJsonObject &json)
{
    QJsonDocument doc;
    doc.setObject(json);
    tcpSocket->write(doc.toJson());
    tcpSocket->flush();
}

void SocketHandler::on_tcpSocket_disconnected()
{
    qDebug() << " tcpsocket disconnected";
    if(process && process->state() != QProcess::NotRunning)
    {
        process->kill();
        process->deleteLater();
    }
//    localSocket->deleteLater();
//    localServer->deleteLater();
//    tcpSocket->deleteLater();
}

void SocketHandler::start()
{
    while(1)
    {
        if(tcpSocket->state() == QAbstractSocket::UnconnectedState || tcpSocket->waitForDisconnected(1000))
        {
            qDebug() << "break .....  ";
            break;
        }
        else
        {
            qDebug() << "continue ....  ";
        }
    }
}
