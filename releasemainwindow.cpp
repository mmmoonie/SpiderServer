#include "releasemainwindow.h"
#include "ui_releasemainwindow.h"
#include <QMessageBox>
#include <QPalette>
#include <QDateTime>

ReleaseMainWindow::ReleaseMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReleaseMainWindow)
{
    ui->setupUi(this);
    tcpServer = new TcpServer(this);
    ui->plainTextEditLog->setStyleSheet("background-color:black;font-size:16px;");
    QPalette palette = ui->plainTextEditLog->palette();
    palette.setColor(QPalette::Active, QPalette::Base, Qt::white);
    palette.setColor(QPalette::Inactive, QPalette::Base, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    ui->plainTextEditLog->setPalette(palette);
    statusLabel = new QLabel(this);
    statusLabel->setText("waitting for start");
    this->statusBar()->addWidget(statusLabel);
}

ReleaseMainWindow::~ReleaseMainWindow()
{
    delete ui;
}

void ReleaseMainWindow::on_pushButtonStart_clicked()
{
    QString status = ui->pushButtonStart->text();
    if(status == "start")
    {
        int port = ui->lineEditPort->text().toInt();
        if(!tcpServer->listen(QHostAddress::Any, port))
        {
            QMessageBox::warning(this, "warn", QString("can't listen on ").append(port));
        }
        else
        {
            ui->pushButtonStart->setText("close");
            statusLabel->setText("waitting for connection");
        }
    }
    else
    {
        tcpServer->close();
        ui->pushButtonStart->setText("start");
        statusLabel->setText("waitting for start");
    }
}

void ReleaseMainWindow::on_pushButtonClear_clicked()
{
    ui->plainTextEditLog->clear();
}
