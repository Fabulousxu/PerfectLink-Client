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
	creatRoomWindow = new CreateRoomWindow(this);
	roomInfomationWindow = new RoomInfomationWindow(this);
	errorShowTimer = new QTimer(this);
	errorShowTimer->setSingleShot(true);
	errorShowTimer->setInterval(3000);
	connect(errorShowTimer, &QTimer::timeout, this, [this] { ui->errorLabel->clear(); });
	connect(ui->modeButton, &QPushButton::clicked, this, &HomeWindow::onModeButton);
	connect(ui->logoffButton, &QPushButton::clicked, this, [this] { emit logoffRequest(); });
}

void HomeWindow::setAccountInfomation(quint64 id, const QString &nickname)
{
	ui->idLabel->setText(QString::number(id));
	ui->nicknameLabel->setText(nickname);
}

void HomeWindow::onLogoffSuccess()
{
	emit logoffSuccess();
	ui->idLabel->clear();
	ui->nicknameLabel->clear();
	mode = 1;
	ui->modeButton->setText("双人对战");
}

void HomeWindow::onModeButton() {
	switch (mode) {
		case 1:
			mode = 2;
			ui->modeButton->setText("三人对战");
			break;
		case 2:
			mode = 3;
			ui->modeButton->setText("四人对战");
			break;
		case 3:
			mode = 1;
			ui->modeButton->setText("双人对战");
			break;
	}
}

void HomeWindow::onCreatRoomButton()
{
}

void HomeWindow::onMatchRoomButton()
{
}

void HomeWindow::showError(const QString &error) {
	ui->errorLabel->setText(error);
	errorShowTimer->start();
}
