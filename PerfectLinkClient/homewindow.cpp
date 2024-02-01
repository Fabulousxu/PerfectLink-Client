#include "homewindow.h"

HomeWindow::HomeWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::HomeWindowClass())
{
	ui->setupUi(this);
	setParent(parent);
	setFixedSize(parent->size());
	move(0, 0);
	setAttribute(Qt::WA_StyledBackground);
	mode = 1;
	createRoomWindow = new CreateRoomWindow(this);
	roomInfomationWindow = new RoomInfomationWindow(this);
	createRoomWindow->hide();
	roomInfomationWindow->hide();
	errorShowTimer = new QTimer(this);
	errorShowTimer->setSingleShot(true);
	errorShowTimer->setInterval(3000);
	connect(errorShowTimer, &QTimer::timeout, this, [this] { ui->errorLabel->clear(); });
	connect(ui->modeButton, &QPushButton::clicked, this, &HomeWindow::onModeButton);
	connect(ui->logoffButton, &QPushButton::clicked, this, &HomeWindow::onLogoffButton);
	connect(ui->createRoomButton, &QPushButton::clicked, this, &HomeWindow::onCreatRoomButton);
	connect(ui->matchRoomButton, &QPushButton::clicked, this, &HomeWindow::onMatchRoomButton);
	connect(createRoomWindow, &CreateRoomWindow::backToHome, this, &HomeWindow::onBackFromCreateRoomWindow);
	connect(roomInfomationWindow, &RoomInfomationWindow::backToHome, this, &HomeWindow::onBackFromRoomInfomationWindow);
}

void HomeWindow::setAccountInfomation(quint64 id, const QString &nickname)
{
	ui->idLabel->setText(QString::number(id));
	ui->nicknameLabel->setText(nickname);
}

void HomeWindow::onLogoffButton()
{
	emit logoffRequest();
}

void HomeWindow::onLogoffSuccess()
{
	emit logoffSuccess();
	ui->idLabel->clear();
	ui->nicknameLabel->clear();
	mode = 1;
	ui->modeButton->setText("双人对战");
}

void HomeWindow::onModeButton()
{
	setMode(mode < 3 ? mode + 1 : 1);
}

void HomeWindow::onBackFromCreateRoomWindow(int mode)
{
	setMode(mode);
	createRoomWindow->hide();
}

void HomeWindow::onBackFromRoomInfomationWindow(int mode)
{
	setMode(mode);
	roomInfomationWindow->hide();
}

void HomeWindow::onCreatRoomButton()
{
	createRoomWindow->setMode(mode);
	createRoomWindow->setLevel(0);
	createRoomWindow->show();
}

void HomeWindow::onMatchRoomButton()
{
	roomInfomationWindow->setMode(mode);
	roomInfomationWindow->show();
	emit roomInfomationWindow->requireRoomRequest(mode + 1);
}

void HomeWindow::setMode(int m)
{
	if (0 <= mode && mode <= 4) { mode = m; }
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

void HomeWindow::showError(const QString &error) {
	ui->errorLabel->setText(error);
	errorShowTimer->start();
}
