#include "roominfomationwindow.h"

RoomInfomationWindow::RoomInfomationWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::RoomInfomationWindowClass()) {
  ui->setupUi(this);
  setParent(parent);
  setFixedSize(parent->size());
  move(0, 0);
  setAttribute(Qt::WA_StyledBackground);
  setFocusPolicy(Qt::StrongFocus);
  ui->roomIdInput->setValidator(
      new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
  errorShowTimer = new QTimer(this);
  errorShowTimer->setSingleShot(true);
  errorShowTimer->setInterval(3000);
  mode = 1;
  roomDisplay.append(ui->room1Display);
  roomDisplay.append(ui->room2Display);
  roomDisplay.append(ui->room3Display);
  roomDisplay.append(ui->room4Display);
  roomDisplay.append(ui->room5Display);
  roomIdLabel.append(ui->room1IdLabel);
  roomIdLabel.append(ui->room2IdLabel);
  roomIdLabel.append(ui->room3IdLabel);
  roomIdLabel.append(ui->room4IdLabel);
  roomIdLabel.append(ui->room5IdLabel);
  roomPlayerNumberLabel.append(ui->room1PlayerNumberLabel);
  roomPlayerNumberLabel.append(ui->room2PlayerNumberLabel);
  roomPlayerNumberLabel.append(ui->room3PlayerNumberLabel);
  roomPlayerNumberLabel.append(ui->room4PlayerNumberLabel);
  roomPlayerNumberLabel.append(ui->room5PlayerNumberLabel);
  playerNumberLabel.append(ui->playerNumber1Label);
  playerNumberLabel.append(ui->playerNumber2Label);
  playerNumberLabel.append(ui->playerNumber3Label);
  playerNumberLabel.append(ui->playerNumber4Label);
  playerNumberLabel.append(ui->playerNumber5Label);
  enterRoomButton.append(ui->enterRoom1Button);
  enterRoomButton.append(ui->enterRoom2Button);
  enterRoomButton.append(ui->enterRoom3Button);
  enterRoomButton.append(ui->enterRoom4Button);
  enterRoomButton.append(ui->enterRoom5Button);
  for (auto pRoomDiaplay : roomDisplay) {
    pRoomDiaplay->hide();
  }
  connect(errorShowTimer, &QTimer::timeout, this,
          [this] { ui->errorLabel->clear(); });
  connect(this, &RoomInfomationWindow::requireRoomRequest, this,
          [this](int pn) { showError("获取房间信息中..."); });
  connect(ui->modeButton, &QPushButton::clicked, this,
          &RoomInfomationWindow::onModeButton);
  connect(ui->backButton, &QPushButton::clicked, this,
          &RoomInfomationWindow::onBackButton);
  connect(ui->refreshButton, &QPushButton::clicked, this,
          &RoomInfomationWindow::onRefreshButton);
  connect(ui->roomIdInput, &QLineEdit::textChanged, this,
          &RoomInfomationWindow::onRoomIdInput);
  for (int i = 0; i < 5; ++i) {
    connect(enterRoomButton[i], &QPushButton::clicked, this,
            [this, i] { onEnterRoomButton(i); });
  }
}

void RoomInfomationWindow::setMode(int m) {
  if (0 <= mode && mode <= 4) {
    mode = m;
  }
  if (mode == 0) {
    ui->modeButton->setText("单人模式");
  } else if (mode == 1) {
    ui->modeButton->setText("双人对战");
  } else if (mode == 2) {
    ui->modeButton->setText("三人对战");
  } else if (mode == 3) {
    ui->modeButton->setText("四人对战");
  }
}

void RoomInfomationWindow::onRequireRoomSuccess(
    const QVector<QPair<quint64, int>> &roomInfomation) {
  showError("房间信息获取成功");
  int i;
  for (i = 0; i < qMin(roomInfomation.size(), 5); ++i) {
    roomIdLabel[i]->setText(QString::number(roomInfomation[i].first));
    roomPlayerNumberLabel[i]->setText(
        QString::number(roomInfomation[i].second));
    playerNumberLabel[i]->setText(QString::number(mode + 1));
    roomDisplay[i]->show();
  }
  for (; i < 5; ++i) {
    roomDisplay[i]->hide();
  }
}

void RoomInfomationWindow::showError(const QString &error) {
  ui->errorLabel->setText(error);
  errorShowTimer->start();
}

void RoomInfomationWindow::onRequireRoomFail(const QString &error) {
  showError("获取房间信息失败");
}

void RoomInfomationWindow::onEnterRoomSuccess() {
  for (auto button : enterRoomButton) {
    button->setEnabled(true);
  }
  ui->roomIdInput->setReadOnly(false);
  ui->backButton->setEnabled(true);
}

void RoomInfomationWindow::onEnterRoomFail(const QString &error) {
  showError(error);
  for (auto button : enterRoomButton) {
    button->setEnabled(true);
  }
  ui->roomIdInput->setReadOnly(false);
  ui->backButton->setEnabled(true);
}

void RoomInfomationWindow::onModeButton() {
  setMode(mode < 3 ? mode + 1 : 1);
  emit requireRoomRequest(mode + 1);
}

void RoomInfomationWindow::onBackButton() {
  for (auto pRoomDiaplay : roomDisplay) {
    pRoomDiaplay->hide();
  }
  emit backToHome(mode);
}

void RoomInfomationWindow::onRefreshButton() {
  emit requireRoomRequest(mode + 1);
}

void RoomInfomationWindow::onRoomIdInput() {
  showError("查找房间中...");
  emit searchRoomRequest(ui->roomIdInput->text().toULongLong());
}

void RoomInfomationWindow::onEnterRoomButton(int number) {
  showError("进入房间中...");
  for (auto button : enterRoomButton) {
    button->setEnabled(false);
  }
  ui->roomIdInput->setReadOnly(true);
  ui->backButton->setEnabled(false);
  emit enterRoomRequest(roomIdLabel[number]->text().toULongLong());
}
