#include "startwindow.h"

StartWindow::StartWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::StartWindowClass()) {
  ui->setupUi(this);
  setParent(parent);
  setFixedSize(parent->size());
  move(0, 0);
  setAttribute(Qt::WA_StyledBackground);
  setFocusPolicy(Qt::StrongFocus);
  flag = true;
  ui->idInput->setValidator(
      new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
  errorShowTimer = new QTimer(this);
  errorShowTimer->setSingleShot(true);
  errorShowTimer->setInterval(3000);
  connect(errorShowTimer, &QTimer::timeout, this,
          [this] { ui->errorLabel->clear(); });
  connect(ui->shiftButton, &QPushButton::clicked, this,
          &StartWindow::onShiftButton);
  connect(ui->submitButton, &QPushButton::clicked, this,
          &StartWindow::onSubmitButton);
  connect(ui->idInput, &QLineEdit::returnPressed, this,
          [this] { ui->passwordInput->setFocus(); });
  connect(ui->passwordInput, &QLineEdit::returnPressed, this,
          &StartWindow::onSubmitButton);
}

void StartWindow::onSignupSuccess(quint64 id) {
  showError("注册成功! 您的账号为" + QString::number(id));
  ui->idInput->setReadOnly(false);
  ui->passwordInput->setReadOnly(false);
  ui->submitButton->setEnabled(true);
  ui->shiftButton->setEnabled(true);
}

void StartWindow::onSignupFail(const QString &error) {
  showError("注册失败! 原因: " + error);
  ui->idInput->setReadOnly(false);
  ui->passwordInput->setReadOnly(false);
  ui->submitButton->setEnabled(true);
  ui->shiftButton->setEnabled(true);
}

void StartWindow::onLoginSuccess(const QString &nickname) {
  auto id = ui->idInput->text().toULongLong();
  ui->idInput->clear();
  ui->passwordInput->clear();
  ui->errorLabel->clear();
  ui->idInput->setReadOnly(false);
  ui->passwordInput->setReadOnly(false);
  ui->submitButton->setEnabled(true);
  ui->shiftButton->setEnabled(true);
  emit loginSuccess(id, nickname);
}

void StartWindow::onLoginFail(const QString &error) {
  showError("登录失败! 原因: " + error);
  ui->passwordInput->clear();
  ui->idInput->setReadOnly(false);
  ui->passwordInput->setReadOnly(false);
  ui->submitButton->setEnabled(true);
  ui->shiftButton->setEnabled(true);
}

void StartWindow::onShiftButton() {
  ui->idInput->clear();
  ui->passwordInput->clear();
  if (flag) {
    ui->idLabel->setText("昵称  ");
    ui->passwordInput->setEchoMode(QLineEdit::Normal);
    ui->shiftButton->setText("登录账号");
    ui->submitButton->setText("注册");
    ui->idInput->setValidator(new QRegularExpressionValidator());
  } else {
    ui->idLabel->setText("账号  ");
    ui->passwordInput->setEchoMode(QLineEdit::Password);
    ui->shiftButton->setText("注册账号");
    ui->submitButton->setText("登录");
    ui->idInput->setValidator(
        new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
  }
  flag = !flag;
}

void StartWindow::onSubmitButton() {
  auto id = ui->idInput->text();
  auto password = ui->passwordInput->text();
  if (password.isEmpty()) {
    showError("密码不能为空!");
    return;
  }
  ui->idInput->setReadOnly(true);
  ui->passwordInput->setReadOnly(true);
  ui->submitButton->setEnabled(false);
  ui->shiftButton->setEnabled(false);
  if (flag) {
    if (id.isEmpty()) {
      showError("账号不能为空!");
      return;
    }
    ui->errorLabel->setText("登录中...");
    emit loginRequest(id.toULongLong(), password);
  } else {
    if (id.isEmpty()) {
      showError("昵称不能为空!");
      return;
    }
    ui->errorLabel->setText("注册中...");
    emit signupRequest(id, password);
  }
}

void StartWindow::showError(const QString &error) {
  ui->errorLabel->setText(error);
  errorShowTimer->start();
}

//加背景会出bug

//extern QPixmap *background;
//
//void StartWindow::paintEvent(QPaintEvent *event) {
//  QPainter *painter = new QPainter(this);
//  painter->drawPixmap(0, 0, height(), width(), *background);
//  delete painter;
//}

