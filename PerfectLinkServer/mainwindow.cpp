#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playerinfo.h"

//TODO: Address? Port?
#define SERVER_PORT 11080

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
    , server(new QTcpServer(this))
{
    ui->setupUi(this);

    PlayerInfo::load();
    PlayerSocket::setWidget(ui->userTable,ui->stateDisplay);
    server->listen(QHostAddress::Any, SERVER_PORT);
    connect(server,&QTcpServer::newConnection, this, &MainWindow::onNewConnection);
}

void MainWindow::onNewConnection()//来了新的连接
{
    //获取与之对话的socket
    auto sock=server->nextPendingConnection();
    PlayerSocket *client=new PlayerSocket(sock,this);
    clients.append(client);
    //TODO: 记录用户变化
}

MainWindow::~MainWindow()
{
    delete ui;
}

