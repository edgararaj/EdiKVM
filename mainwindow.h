#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onNewConnection();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void onReadyRead_server();
    void onReadyRead_client();

private slots:
    void on_radioButton_client_clicked(bool checked);

    void on_radioButton_server_clicked(bool checked);

    void on_pushButton_start_clicked();

    void on_pushButton_stop_clicked();

private:
    enum ConnectionType {
        Server, Client
    } connectionType;
    void setConnectionType(ConnectionType);
    Ui::MainWindow *ui;
    QTcpServer server;
    QTcpSocket client;
    QList<QTcpSocket*> sockets;
};
#endif // MAINWINDOW_H
