#include "mainwindow.h"

QPixmap *background = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainWindowClass()) {
  ui->setupUi(this);
   //showFullScreen();
  resize(1280, 720);
  socket = new Socket(this);
  startWindow = new StartWindow(this);
  homeWindow = new HomeWindow(this);
  gameWindow = new GameWindow(this);
  endWindow = new EndWindow(this);
  homeWindow->hide();
  gameWindow->hide();
  endWindow->hide();

  accountInfomation.id = 10001;
  accountInfomation.nickname = "东川路徐先生";

  /* 注册处理 */
  connect(startWindow, &StartWindow::signupRequest, socket,
          &Socket::onSignupRequest);
  connect(socket, &Socket::signupSuccess, startWindow,
          &StartWindow::onSignupSuccess);
  connect(socket, &Socket::signupFail, startWindow, &StartWindow::onSignupFail);

  /* 登录处理 */
  connect(startWindow, &StartWindow::loginRequest, socket,
          &Socket::onLoginRequest);
  connect(socket, &Socket::loginSuccess, startWindow,
          &StartWindow::onLoginSuccess);
  connect(socket, &Socket::loginFail, startWindow, &StartWindow::onLoginFail);
  connect(startWindow, &StartWindow::loginSuccess, this,
          &MainWindow::onLoginSuccess);

  /* 登出处理 */
  connect(homeWindow, &HomeWindow::logoffRequest, socket,
          [this] { socket->onLogoffRequest(accountInfomation.id); });
  connect(socket, &Socket::logoffSuccess, homeWindow,
          &HomeWindow::onLogoffSuccess);
  connect(socket, &Socket::logoffFail, homeWindow, &HomeWindow::onLogoffFail);
  connect(homeWindow, &HomeWindow::logoffSuccess, this,
          &MainWindow::onLogoffSuccess);

  /* 创建房间处理 */
  connect(homeWindow->createRoomWindow, &CreateRoomWindow::createRoomRequest,
          gameWindow, &GameWindow::setParameter);
  connect(homeWindow->createRoomWindow, &CreateRoomWindow::createRoomRequest,
          socket, &Socket::onCreateRoomRequest);
  connect(socket, &Socket::createRoom, homeWindow->createRoomWindow,
          &CreateRoomWindow::onCreateRoomSuccess);
  connect(homeWindow->createRoomWindow, &CreateRoomWindow::createRoomSuccess,
          this, &MainWindow::onCreateRoomSuccess);

  /* 请求房间处理 */
  connect(homeWindow->roomInfomationWindow,
          &RoomInfomationWindow::requireRoomRequest, socket,
          &Socket::onRequireRoomRequest);
  connect(socket, &Socket::requireRoom, homeWindow->roomInfomationWindow,
          &RoomInfomationWindow::onRequireRoomSuccess);

  /* 进入房间处理 */
  connect(homeWindow->roomInfomationWindow,
          &RoomInfomationWindow::enterRoomRequest, socket,
          &Socket::onEnterRoomRequest);
  connect(homeWindow->roomInfomationWindow,
          &RoomInfomationWindow::enterRoomRequest, this,
          [this](quint64 rid) { roomId = rid; });
  connect(socket, &Socket::enterRoomSuccess, this,
          &MainWindow::onEnterRoomSuccess);
  connect(socket, &Socket::enterRoomFail, homeWindow->roomInfomationWindow,
          &RoomInfomationWindow::onEnterRoomFail);

  /* 离开房间处理 */
  connect(gameWindow, &GameWindow::exitRoomRequest, socket,
          &Socket::onExitRoomRequest);
  connect(socket, &Socket::exitRoomSuccess, gameWindow,
          &GameWindow::onExitRoomSuccess);
  connect(socket, &Socket::exitRoomFail, gameWindow,
          &GameWindow::onExitRoomFail);
  connect(gameWindow, &GameWindow::exitRoomSuccess, this,
          &MainWindow::onExitRoomSuccess);

  /* 准备处理 */
  connect(gameWindow, &GameWindow::prepareRequest, socket,
          &Socket::onPrepareRequest);
  connect(socket, &Socket::playerPrepare, gameWindow,
          [this](quint64 id, bool prepare) {
            gameWindow->onPrepare(id, prepare, accountInfomation.id);
          });

  /* 房间人数变动处理 */
  connect(socket, &Socket::playerEnter, gameWindow, &GameWindow::onPlayerEnter);
  connect(socket, &Socket::playerExit, gameWindow, &GameWindow::onPlayerExit);

  /* 游戏处理 */
  connect(socket, &Socket::gameBegin, gameWindow, &GameWindow::onGameBegin);
  connect(gameWindow, &GameWindow::moveRequest, socket, &Socket::onMoveRequest);
  connect(socket, &Socket::playerMove, gameWindow,
          [this](quint64 id, Direction d, bool flag) {
            gameWindow->gameCanvas->movePlayer(id, d, flag);
          });
  connect(socket, &Socket::selectBlock, gameWindow,
          [this](const QPoint &p, quint64 id) {
            gameWindow->gameCanvas->selectBlock(p, id);
          });
  connect(socket, &Socket::unSelectBlock, gameWindow, [this](const QPoint &p) {
    gameWindow->gameCanvas->unSelectBlock(p);
  });
  connect(socket, &Socket::drawPath, gameWindow,
          [this](const QVector<QPoint> &path) {
            gameWindow->gameCanvas->drawPath(path);
          });
  connect(socket, &Socket::mark, gameWindow, &GameWindow::onMark);

  /* 游戏结束处理 */
  connect(socket, &Socket::gameEnd, gameWindow,
          [this](const QVector<QPair<quint64, int>> &rank) {
            gameWindow->onGameEnd(rank, accountInfomation.id);
          });
  connect(gameWindow, &GameWindow::gameEnd, this, &MainWindow::onGameEnd);
  connect(endWindow, &EndWindow::backToHome, this,
          &MainWindow::onBackToHomeFromEnd);

  socket->connectToHost(SERVER_IP, SERVER_PORT);
  startWindow->show();
  // homeWindow->show();
  // gameWindow->show();
  // gameWindow->setFocus();
}

void MainWindow::onLoginSuccess(quint64 id, const QString &nickname) {
  homeWindow->setAccountInfomation(id, nickname);
  startWindow->hide();
  homeWindow->show();
  accountInfomation.id = id;
  accountInfomation.nickname = nickname;
}

void MainWindow::onLogoffSuccess() {
  homeWindow->hide();
  startWindow->show();
}

void MainWindow::onCreateRoomSuccess(quint64 rid) {
  homeWindow->createRoomWindow->hide();
  homeWindow->hide();
  gameWindow->onCreateRoomSuccess(rid, accountInfomation.id,
                                  accountInfomation.nickname);
  gameWindow->show();
}

void MainWindow::onEnterRoomSuccess(
    int playerLimit, int width, int height, int patternNumber, int time,
    const QVector<QPair<quint64, QPair<QString, bool>>> &playerInfomation) {
  homeWindow->roomInfomationWindow->onEnterRoomSuccess();
  homeWindow->roomInfomationWindow->hide();
  homeWindow->hide();
  gameWindow->setParameter(playerLimit, width, height, patternNumber, time);
  gameWindow->show();
  gameWindow->onEnterRoomSuccess(playerInfomation, roomId, accountInfomation.id,
                                 accountInfomation.nickname);
}

void MainWindow::onExitRoomSuccess() {
  gameWindow->hide();
  homeWindow->show();
}

void MainWindow::onGameEnd(const QVector<QPair<QString, int>> &rank, int self) {
  gameWindow->hide();
  endWindow->onGameEnd(rank, self);
  endWindow->show();
}

void MainWindow::onBackToHomeFromEnd() {
  endWindow->hide();
  homeWindow->show();
}
