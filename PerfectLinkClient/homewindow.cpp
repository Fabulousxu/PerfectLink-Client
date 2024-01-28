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
	state = 2;
	errorShowTimer = new QTimer(this);
	errorShowTimer->setSingleShot(true);
	errorShowTimer->setInterval(3000);
	connect(errorShowTimer, &QTimer::timeout, this, [this] { ui->errorLabel->clear(); });
	connect(ui->modeButton, &QPushButton::clicked, this, &HomeWindow::onModeButton);
	connect(ui->logoffButton, &QPushButton::clicked, this, [this] { emit logoffRequest(); });
}

HomeWindow::~HomeWindow()
{
	delete ui;
}

void HomeWindow::setAccountMessage(quint64 id, const QString &nickname) {
	ui->idLabel->setText(QString::number(id));
	ui->nicknameLabel->setText(nickname);
}

void HomeWindow::onLogoffSuccess() {
	emit logoffSuccess();
	ui->idLabel->clear();
	ui->nicknameLabel->clear();
	state = 1;
	ui->modeButton->setText("双人对战");
}

void HomeWindow::onLogoffFail(const QString &error) {
	showError(error);
}

void HomeWindow::onModeButton() {
	switch (state) {
		case 1:
			state = 2;
			ui->modeButton->setText("三人对战");
			break;
		case 2:
			state = 3;
			ui->modeButton->setText("四人对战");
			break;
		case 3:
			state = 1;
			ui->modeButton->setText("双人对战");
			break;
	}
}

void HomeWindow::showError(const QString &error) {
	ui->errorLabel->setText(error);
	errorShowTimer->start();
}
