#include <QThread>
#include "tcpserver.h"
#include "socketthread.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{

}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    SocketThread * socketThread = new SocketThread(NULL, socketDescriptor);
    connect(socketThread, &SocketThread::finished, socketThread, &QThread::deleteLater);
    socketThread->start();
}
