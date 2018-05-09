#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLabel>
#include "tcpserver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void on_tcpServer_newConnection();
    void on_tcpSocket_readyRead();
    void on_tcpSocket_aboutToClose();

public slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonSend_clicked();


private slots:
    void on_pushButtonClear_clicked();

private:
    Ui::MainWindow *ui;
    QLabel * statusLabel;
    QTcpServer * tcpServer;
    QTcpSocket * tcpSocket;
};

#endif // MAINWINDOW_H
