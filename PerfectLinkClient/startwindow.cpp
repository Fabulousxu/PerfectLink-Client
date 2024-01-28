#include "startwindow.h"

StartWindow::StartWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::StartWindowClass())
{
	ui->setupUi(this);
	setParent(parent);
	setFixedSize(parent->size());
	move(0, 0);
	setAttribute(Qt::WA_StyledBackground);
	setFocusPolicy(Qt::StrongFocus);
	flag = true;
	ui->idInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	errorShowTimer = new QTimer(this);
	errorShowTimer->setSingleShot(true);
	errorShowTimer->setInterval(3000);
	connect(errorShowTimer, &QTimer::timeout, this, [this] { ui->errorLabel->clear(); });
	connect(ui->shiftButton, &QPushButton::clicked, this, &StartWindow::onShiftButton);
	connect(ui->submitButton, &QPushButton::clicked, this, &StartWindow::onSubmitButton);
	connect(ui->idInput, &QLineEdit::returnPressed, this, [this] { ui->passwardInput->setFocus(); });
	connect(ui->passwardInput, &QLineEdit::returnPressed, this, &StartWindow::onSubmitButton);
}

StartWindow::~StartWindow()
{
	delete ui;
}

void StartWindow::onSignupSuccess(quint64 id) {
	showError("注册成功!您的账号为" + QString::number(id));
	ui->idInput->setReadOnly(false);
	ui->passwardInput->setReadOnly(false);
}

void StartWindow::onSignupFail(const QString &error) {
	showError("注册失败!原因:" + error);
	ui->idInput->setReadOnly(false);
	ui->passwardInput->setReadOnly(false);
}

void StartWindow::onLoginSuccess(const QString &nickname) {
	emit loginSuccess(ui->idInput->text().toULongLong(), nickname);
	ui->idInput->clear();
	ui->passwardInput->clear();
	ui->errorLabel->clear();
	ui->idInput->setReadOnly(false);
	ui->passwardInput->setReadOnly(false);
}

void StartWindow::onLoginFail(const QString &error) {
	showError("登录失败!原因:" + error);
	ui->passwardInput->clear();
	ui->idInput->setReadOnly(false);
	ui->passwardInput->setReadOnly(false);
}

void StartWindow::onShiftButton() {
	ui->idInput->clear();
	ui->passwardInput->clear();
	if (flag) {
		ui->idLabel->setText("昵称");
		ui->passwardInput->setEchoMode(QLineEdit::Normal);
		ui->shiftButton->setText("登录账号");
		ui->submitButton->setText("注册");
		ui->idInput->setValidator(new QRegularExpressionValidator());
	} else {
		ui->idLabel->setText("账号");
		ui->passwardInput->setEchoMode(QLineEdit::Password);
		ui->shiftButton->setText("注册账号");
		ui->submitButton->setText("登录");
		ui->idInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	}
	flag = !flag;
}

void StartWindow::onSubmitButton() {
	if (flag) {
		const QString &id = ui->idInput->text();
		const QString &passward = ui->passwardInput->text();
		if (id.isEmpty()) {
			showError("账号不能为空!");
			return;
		}
		if (passward.isEmpty()) {
			showError("密码不能为空!");
			return;
		}
		ui->errorLabel->setText("登录中...");
		ui->idInput->setReadOnly(true);
		ui->passwardInput->setReadOnly(true);
		emit loginRequest(id.toULongLong(), passward);
	} else {
		const QString &nickname = ui->idInput->text();
		const QString &passward = ui->passwardInput->text();
		if (nickname.isEmpty()) {
			showError("昵称不能为空!");
			return;
		}
		if (passward.isEmpty()) {
			showError("密码不能为空!");
			return;
		}
		ui->errorLabel->setText("注册中...");
		ui->idInput->setReadOnly(true);
		ui->passwardInput->setReadOnly(true);
		emit signupRequest(nickname, passward);
	}
}

void StartWindow::showError(const QString &error) {
	ui->errorLabel->setText(error);
	errorShowTimer->start();
}

