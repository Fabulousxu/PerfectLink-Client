#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindowClass())
{
    ui->setupUi(this);
    showFullScreen();
    socket = new Socket(this);
    startWindow = new StartWindow(this);
    homeWindow = new HomeWindow(this);
    gameWindow = new GameWindow(this);

    /* 注册处理 */
    connect(startWindow, &StartWindow::signupRequest, socket, &Socket::onSignupRequest);
    connect(socket, &Socket::signupSuccess, startWindow, &StartWindow::onSignupSuccess);
    connect(socket, &Socket::signupFail, startWindow, &StartWindow::onSignupFail);

    /* 登录处理 */
    connect(startWindow, &StartWindow::loginRequest, socket, &Socket::onLoginRequest);
    connect(socket, &Socket::loginSuccess, startWindow, &StartWindow::onLoginSuccess);
    connect(socket, &Socket::loginFail, startWindow, &StartWindow::onLoginFail);
    connect(startWindow, &StartWindow::loginSuccess, this, &MainWindow::onLoginSuccess);

    /* 登出处理 */
    connect(homeWindow, &HomeWindow::logoffRequest, socket, [this] { socket->onLogoffRequest(accountInfomation.id); });
    connect(socket, &Socket::logoffSuccess, homeWindow, &HomeWindow::onLogoffSuccess);
    connect(socket, &Socket::logoffFail, homeWindow, &HomeWindow::onLogoffFail);
    connect(homeWindow, &HomeWindow::logoffSuccess, this, &MainWindow::onLogoffSuccess);

    /* 创建房间处理 */
    connect(homeWindow->createRoomWindow, &CreateRoomWindow::createRoomRequest, socket, &Socket::onCreateRoomRequest);
    connect(socket, &Socket::createRoom, homeWindow->createRoomWindow, &CreateRoomWindow::onCreateRoomSuccess);
    connect(homeWindow->createRoomWindow, &CreateRoomWindow::createRoomSuccess, this, &MainWindow::onCreateRoomSuccess);

    socket->connectToHost(SERVER_IP, SERVER_PORT);
    startWindow->show();
    //homeWindow->show();

}

void MainWindow::onLoginSuccess(quint64 id, const QString &nickname) {
    homeWindow->setAccountInfomation(id, nickname);
    startWindow->hide();
    homeWindow->show();
    accountInfomation.id = id;
    accountInfomation.nickname = nickname;
}

void MainWindow::onLogoffSuccess()
{
    homeWindow->hide();
    startWindow->show();
}

void MainWindow::onCreateRoomSuccess()
{
    homeWindow->createRoomWindow->hide();
    homeWindow->hide();
    gameWindow->show();
}
