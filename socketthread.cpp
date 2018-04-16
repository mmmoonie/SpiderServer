#include "socketthread.h"

SocketThread::SocketThread(QObject *parent, qintptr descriptor) : QThread(parent)
{
    this->socketDescriptor = descriptor;
}

void SocketThread::run()
{

}

SocketThread::~SocketThread()
{

}
