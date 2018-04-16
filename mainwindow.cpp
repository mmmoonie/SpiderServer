#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QJsonParseError>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    localServer = new QLocalServer(this);
    connect(localServer, &QLocalServer::newConnection, this, &MainWindow::on_localServer_newConnection);
    statusLabel = new QLabel(this);
    statusLabel->setText("waitting for start");
    this->statusBar()->addWidget(statusLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonStart_clicked()
{
    QString status = ui->pushButtonStart->text();
    if(status == "start")
    {
        QString serverName = ui->lineEditPort->text();
        if(!localServer->listen(serverName))
        {
            QMessageBox::warning(this, "warn", QString("can't listen on ").append(serverName));
        }
        else
        {
            ui->pushButtonSend->setEnabled(true);
            ui->pushButtonStart->setText("close");
            statusLabel->setText("waitting for connection");
            ui->textEditSend->setFocus();
        }
    }
    else
    {
        localServer->close();
        ui->pushButtonStart->setText("start");
        statusLabel->setText("waitting for start");
        ui->pushButtonSend->setEnabled(false);
    }

}

void MainWindow::on_localServer_newConnection()
{
    localSocket = localServer->nextPendingConnection();
    connect(localSocket, &QLocalSocket::readyRead, this, &MainWindow::on_localSocket_readyRead);
    connect(localSocket, &QLocalSocket::aboutToClose, this, &MainWindow::on_localSocket_aboutToClose);
    ui->pushButtonSend->setEnabled(true);
}

void MainWindow::on_localSocket_readyRead()
{
    QByteArray data = localSocket->readAll();
    qDebug() << data;
    QJsonParseError * parseError = new QJsonParseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, parseError);
    if(parseError->error != QJsonParseError::NoError || jsonDoc.isNull())
    {
        QMessageBox::warning(this, "warn", "parse json error");
        return;
    }
    ui->textEditReceive->setText(jsonDoc.toJson());
}

void MainWindow::on_localSocket_aboutToClose()
{
    localSocket->close();
    delete localSocket;
    ui->pushButtonSend->setEnabled(false);
}

void MainWindow::on_pushButtonSend_clicked()
{
    QString data = ui->textEditSend->toPlainText();
    if(data.isEmpty())
    {
        QMessageBox::warning(this, "warn", "content is empty");
    }
    else
    {
        localSocket->write(data.toUtf8());
        localSocket->flush();
        localSocket->waitForBytesWritten();
    }
}

void MainWindow::on_pushButtonClear_clicked()
{
    ui->textEditReceive->clear();
}
