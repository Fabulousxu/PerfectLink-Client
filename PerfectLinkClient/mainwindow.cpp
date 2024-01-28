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

    connect(startWindow, &StartWindow::signupRequest, socket, &Socket::onSignupRequest);
    connect(startWindow, &StartWindow::loginRequest, socket, &Socket::onLoginRequest);
    connect(socket, &Socket::signupSuccess, startWindow, &StartWindow::onSignupSuccess);
    connect(socket, &Socket::signupFail, startWindow, &StartWindow::onSignupFail);
    connect(socket, &Socket::loginSuccess, startWindow, &StartWindow::onLoginSuccess);
    connect(socket, &Socket::loginFail, startWindow, &StartWindow::onLoginFail);
    connect(startWindow, &StartWindow::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(homeWindow, &HomeWindow::logoffRequest, this, [this] { socket->onLogoffRequest(playerMessage.id); });
    connect(socket, &Socket::logoffSuccess, homeWindow, &HomeWindow::onLogoffSuccess);
    connect(socket, &Socket::logoffFail, homeWindow, &HomeWindow::onLogoffFail);
    connect(homeWindow, &HomeWindow::logoffSuccess, this, &MainWindow::onLogoffSuccess);

    socket->connectToHost(SERVER_IP, SERVER_PORT);

    startWindow->show();
    //homeWindow->show();
    //gameWindow->show();
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::onLoginSuccess(quint64 id, const QString &nickname) {
    startWindow->hide();
    homeWindow->setAccountMessage(id, nickname);
    homeWindow->show();
    playerMessage.id = id;
    playerMessage.nickname = nickname;
}

void MainWindow::onLogoffSuccess() {
    homeWindow->hide();
    startWindow->show();
}
