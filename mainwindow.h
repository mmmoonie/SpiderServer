#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLocalServer>
#include <QLocalSocket>
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
    void on_localServer_newConnection();
    void on_localSocket_readyRead();
    void on_localSocket_aboutToClose();

public slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonSend_clicked();


private slots:
    void on_pushButtonClear_clicked();

private:
    Ui::MainWindow *ui;
    QLabel * statusLabel;
    QLocalServer * localServer;
    QLocalSocket * localSocket;
};

#endif // MAINWINDOW_H
