#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(QObject *parent = 0, qintptr descriptor = 0);
    void run();
    ~SocketThread();

signals:

public slots:

private:
    qintptr socketDescriptor;
};

#endif // SOCKETTHREAD_H
