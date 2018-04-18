#include <QThread>
#include "tcpserver.h"
#include "sockethandler.h"
#include "socketthread.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{

}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    SocketThread * thread = new SocketThread(socketDescriptor, this);
    connect(thread, &SocketThread::finished, thread, &SocketThread::deleteLater);
    connect(thread, &SocketThread::info, this, &TcpServer::on_SocketHandler_info);
    thread->start();
}

void TcpServer::on_SocketHandler_info(const QString &msg)
{
    emit info(msg);
}
