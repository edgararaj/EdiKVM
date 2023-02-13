#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QNetworkInterface>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    setConnectionType(Server);

    QStringList ips;
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            ips.append(address.toString());
    }
    ui->lineEdit_ip->setText(ips.join("; "));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setConnectionType(ConnectionType c)
{
    connectionType = c;
    switch (c)
    {
    case Server:
        ui->frame_server->setVisible(true);
        ui->frame_client->setVisible(false);
        ui->radioButton_server->setChecked(true);
        ui->radioButton_client->setChecked(false);
        client.close();
        ui->statusbar->showMessage("");
        break;
    case Client:
        ui->frame_server->setVisible(false);
        ui->frame_client->setVisible(true);
        ui->radioButton_server->setChecked(false);
        ui->radioButton_client->setChecked(true);
        server.close();
        ui->statusbar->showMessage("");
        break;
    }
}

void MainWindow::onNewConnection()
{
    QTcpSocket *clientSocket = server.nextPendingConnection();
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead_server()));
    connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

    sockets.push_back(clientSocket);
    ui->statusbar->showMessage(clientSocket->peerAddress().toString() + " connected to server");
    for (QTcpSocket* socket : sockets) {
        socket->write(QByteArray("ola eu sou o servidor"));
    }
}

void MainWindow::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    if (socketState == QAbstractSocket::UnconnectedState)
    {
        QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
        sockets.removeOne(sender);
    }
}

void MainWindow::onReadyRead_server()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray datas = sender->readAll();
    qDebug() << sender->peerAddress().toString() << ": " << QString(datas);
}

void MainWindow::onReadyRead_client()
{
    QByteArray datas = client.readAll();
    qDebug() << "Server: " << QString(datas);
    client.write(QByteArray("eu sei que sim"));
}

void MainWindow::on_radioButton_client_clicked(bool checked)
{
    setConnectionType(checked ? Client : Server);
}


void MainWindow::on_radioButton_server_clicked(bool checked)
{
    setConnectionType(checked ? Server : Client);
}


void MainWindow::on_pushButton_start_clicked()
{
    if (connectionType == Server)
    {
        if (server.listen(QHostAddress::Any, 4242))
        {
            ui->statusbar->showMessage("Server listening");
        }
        else
        {
            ui->statusbar->showMessage(QString("(ERROR): %1").arg(server.errorString()));
        }
    }
    else {
        if (ui->lineEdit_serverIp->text().isEmpty())
        {
            ui->statusbar->showMessage("(ERROR): Please input server ip!");
        }
        else {
            client.connectToHost(ui->lineEdit_serverIp->text(), 4242);
            connect(&client, SIGNAL(readyRead()), this, SLOT(onReadyRead_client()));

            if(client.waitForConnected())
                ui->statusbar->showMessage("Connected to server");
            else{
                ui->statusbar->showMessage(QString("(ERROR): %1").arg(client.errorString()));
            }
        }
    }
}


void MainWindow::on_pushButton_stop_clicked()
{
    if (connectionType == Server)
    {
        server.close();
    }
    else
    {
        client.close();
    }
    ui->statusbar->showMessage("");
}

