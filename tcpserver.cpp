#include <QThread>
#include "tcpserver.h"
#include "sockethandler.h"
#include "socketthread.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
    qRegisterMetaType<tSocketDescriptor>("tSocketDescriptor");
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    SocketThread * thread = new SocketThread(nullptr);
//    connect(thread, &SocketThread::finished, thread, &SocketThread::deleteLater);
////    connect(thread, &SocketThread::info, this, &TcpServer::on_SocketHandler_info);
//    thread->start();

    QMetaObject::invokeMethod(thread, "handleConnection", Qt::QueuedConnection, Q_ARG(tSocketDescriptor, socketDescriptor));
}

void TcpServer::on_SocketHandler_info(const QString msg)
{
    qDebug() << msg;
//    emit info(msg);
}
