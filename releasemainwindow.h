#ifndef RELEASEMAINWINDOW_H
#define RELEASEMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "tcpserver.h"

namespace Ui {
class ReleaseMainWindow;
}

class ReleaseMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReleaseMainWindow(QWidget *parent = 0);
    ~ReleaseMainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButtonClearReq_clicked();

    void on_pushButtonClearRes_clicked();

    void on_pushButtonStart_clicked();

private:
    Ui::ReleaseMainWindow *ui;
    TcpServer * tcpServer;
    QLabel * statusLabel;
};

#endif // RELEASEMAINWINDOW_H
