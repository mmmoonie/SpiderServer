#include <QThread>
#include "tcpserver.h"
#include "sockethandler.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{

}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    SocketHandler * handler = new SocketHandler(0, socketDescriptor);
    connect(handler, &SocketHandler::info, this, &TcpServer::on_SocketHandler_info);
    connect(handler, &SocketHandler::error, this, &TcpServer::on_SocketHandler_error);
    QThread * thread = new QThread(0);
    handler->moveToThread(thread);
    connect(thread, &QThread::started, handler, &SocketHandler::start);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void TcpServer::on_SocketHandler_info(const QString &msg)
{
    emit info(msg);
}

void TcpServer::on_SocketHandler_error(const QString &msg)
{
    emit error(msg);
}
