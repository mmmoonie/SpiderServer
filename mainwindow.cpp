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
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::on_tcpServer_newConnection);
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
        int port = ui->lineEditPort->text().toInt();
        if(!tcpServer->listen(QHostAddress::Any, port))
        {
            QMessageBox::warning(this, "warn", QString("can't listen on ").append(port));
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
        tcpServer->close();
        ui->pushButtonStart->setText("start");
        statusLabel->setText("waitting for start");
        ui->pushButtonSend->setEnabled(false);
    }

}

void MainWindow::on_tcpServer_newConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::on_tcpSocket_readyRead);
    connect(tcpSocket, &QTcpSocket::aboutToClose, this, &MainWindow::on_tcpSocket_aboutToClose);
    ui->pushButtonSend->setEnabled(true);
}

void MainWindow::on_tcpSocket_readyRead()
{
    QByteArray data = tcpSocket->readAll();
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

void MainWindow::on_tcpSocket_aboutToClose()
{
    tcpSocket->close();
    delete tcpSocket;
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
        tcpSocket->write(data.toUtf8());
        tcpSocket->flush();
        tcpSocket->waitForBytesWritten();
    }
}

void MainWindow::on_pushButtonClear_clicked()
{
    ui->textEditReceive->clear();
}
