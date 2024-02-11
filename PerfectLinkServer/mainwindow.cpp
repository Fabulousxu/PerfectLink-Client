#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playerinfo.h"

constexpr quint16 SERVER_PORT=11080; /* 监听端口号 */

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
    , server(new QTcpServer(this))
{
    ui->setupUi(this);
    PlayerInfo::load(); //读取用户数据
    PlayerSocket::setWidget(ui->userTable,ui->stateDisplay); //设置控件
    server->listen(QHostAddress::Any, SERVER_PORT); //监听端口
    connect(server,&QTcpServer::newConnection, this, &MainWindow::onNewConnection); //连接新用户信号
}

void MainWindow::onNewConnection()//来了新的连接
{
    //获取与之对话的socket
    auto sock=server->nextPendingConnection();
    PlayerSocket *client=new PlayerSocket(sock,this);
    clients.append(client);
}

MainWindow::~MainWindow()
{
    delete ui;
}

