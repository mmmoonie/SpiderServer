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
    void on_pushButtonStart_clicked();
    void on_pushButtonClear_clicked();

private:
    Ui::ReleaseMainWindow *ui;
    TcpServer * tcpServer;
    QLabel * statusLabel;
};

#endif // RELEASEMAINWINDOW_H
