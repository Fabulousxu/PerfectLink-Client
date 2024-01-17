#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playersocket.h"

//TODO: Address? Port?
QHostAddress addr("192.168.0.101");

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
    , server(new QTcpServer(this))
{
    ui->setupUi(this);
    //构造函数里直接开启服务器（合理否？） TODO
    server->listen(addr,8080);
    connect(server,&QTcpServer::newConnection, this, &MainWindow::onNewConnection);
}

void MainWindow::onNewConnection()//来了新的连接
{
    //获取与之对话的socket
    PlayerSocket *client=static_cast<PlayerSocket*>(server->nextPendingConnection());
    client->init(ui->userTable,ui->stateDisplay);
    //TODO: 记录用户变化
}

MainWindow::~MainWindow()
{
    delete ui;
}

