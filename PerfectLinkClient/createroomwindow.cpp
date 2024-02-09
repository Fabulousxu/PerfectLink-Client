#include "createroomwindow.h"

CreateRoomWindow::CreateRoomWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CreateRoomWindowClass())
{
	ui->setupUi(this);
	setParent(parent);
	setFixedSize(parent->size());
	move(0, 0);
	setAttribute(Qt::WA_StyledBackground);
	setFocusPolicy(Qt::StrongFocus);
	ui->widthInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	ui->heightInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	ui->patternNumberInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	ui->timeInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	errorShowTimer = new QTimer(this);
	errorShowTimer->setSingleShot(true);
	errorShowTimer->setInterval(3000);
	mode = 1;
	level = 0;
	height = 6;
	width = 6;
	patternNumber = 8;
	time = 90;
	connect(errorShowTimer, &QTimer::timeout, this, [this] { ui->errorLabel->clear(); });
	connect(ui->modeButton, &QPushButton::clicked, this, &CreateRoomWindow::onModeButton);
	connect(ui->backButton, &QPushButton::clicked, this, &CreateRoomWindow::onBackButton);
	connect(ui->levelButton, &QPushButton::clicked, this, &CreateRoomWindow::onLevelButton);
	connect(ui->heightInput, &QLineEdit::editingFinished, this, &CreateRoomWindow::onHeightInput);
	connect(ui->widthInput, &QLineEdit::editingFinished, this, &CreateRoomWindow::onWidthInput);
	connect(ui->patternNumberInput, &QLineEdit::editingFinished, this, &CreateRoomWindow::onPatternNumberInput);
	connect(ui->timeInput, &QLineEdit::editingFinished, this, &CreateRoomWindow::onTimeInput);
	connect(ui->createRoomButton, &QPushButton::clicked, this, &CreateRoomWindow::onCreateRoomButton);
}

void CreateRoomWindow::setMode(int m)
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

void CreateRoomWindow::onModeButton()
{
	setMode(mode < 3 ? mode + 1 : 1);
}

void CreateRoomWindow::showError(const QString &error)
{
	ui->errorLabel->setText(error);
	errorShowTimer->start();
}

bool CreateRoomWindow::setWidth(int w)
{
	bool flag = false;
	if (w < MIN_WIDTH) {
		showError("最小横向图案数量: " + QString::number(MIN_WIDTH));
	} else if (w > MAX_WIDTH) {
		showError("最大横向图案数量: " + QString::number(MAX_WIDTH));
	} else {
		width = w;
		flag = true;
	}
	ui->widthInput->setText(QString::number(width));
	return flag;
}

bool CreateRoomWindow::setHeight(int h)
{
	bool flag = false;
	if (h < MIN_HEIGHT) {
		showError("最小纵向图案数量: " + QString::number(MIN_HEIGHT));
	} else if (h > MAX_HEIGHT) {
		showError("最大纵向图案数量: " + QString::number(MAX_HEIGHT));
	} else {
		height = h;
		flag = true;
	}
	ui->heightInput->setText(QString::number(height));
	return flag;
}

bool CreateRoomWindow::setPatternNumber(int p)
{
	bool flag = false;
	if (p < MIN_PATTERN) {
		showError("最小图案种类数量: " + QString::number(MIN_PATTERN));
	} else if (p > MAX_PATTERN) {
		showError("最大图案种类数量: " + QString::number(MAX_PATTERN));
	} else {
		patternNumber = p;
		flag = true;
	}
	ui->patternNumberInput->setText(QString::number(patternNumber));
	return flag;
}

bool CreateRoomWindow::setTime(int t)
{
	bool flag = false;
	if (t < MIN_TIME) {
		showError("最小游戏时间: " + QString::number(MIN_TIME));
	} else if (t > MAX_TIME) {
		showError("最大游戏时间: " + QString::number(MAX_TIME));
	} else {
		time = t;
		flag = true;
	}		
	ui->timeInput->setText(QString::number(time));
	return flag;
}

void CreateRoomWindow::setLevel(int l)
{
	if (0 <= level && level <= 4) { level = l; }
	if (l == 0) {
		ui->levelButton->setText("简单难度");
		setHeight(6);
		setWidth(6);
		setPatternNumber(8);
		setTime(90);
	} else if (l == 1) {
		ui->levelButton->setText("中等难度");
		setHeight(12);
		setWidth(12);
		setPatternNumber(16);
		setTime(180);
	} else if (l == 2) {
		ui->levelButton->setText("困难难度");
		setHeight(18);
		setWidth(18);
		setPatternNumber(24);
		setTime(360);
	} else if (l == 3) { ui->levelButton->setText("自定义"); }
}

void CreateRoomWindow::onCreateRoomSuccess(quint64 rid)
{
	ui->errorLabel->clear();
	ui->heightInput->setReadOnly(false);
	ui->widthInput->setReadOnly(false);
	ui->patternNumberInput->setReadOnly(false);
	ui->timeInput->setReadOnly(false);
	ui->modeButton->setEnabled(true);
	ui->levelButton->setEnabled(true);
	ui->backButton->setEnabled(true);
	ui->createRoomButton->setEnabled(true);
	emit createRoomSuccess(rid);
}

void CreateRoomWindow::onCreateRoomFail(const QString &error)
{
	showError(error);
	ui->heightInput->setReadOnly(false);
	ui->widthInput->setReadOnly(false);
	ui->patternNumberInput->setReadOnly(false);
	ui->timeInput->setReadOnly(false);
	ui->modeButton->setEnabled(true);
	ui->levelButton->setEnabled(true);
	ui->backButton->setEnabled(true);
	ui->createRoomButton->setEnabled(true);
}

void CreateRoomWindow::onBackButton()
{
	setLevel(0);
	emit backToHome(mode);
}

void CreateRoomWindow::onLevelButton()
{
	setLevel(level < 2 ? level + 1 : 0);
}

void CreateRoomWindow::onCreateRoomButton()
{
	if ((width * height) & 0x1) {
		showError("图案数量必须为偶数个!");
		return;
	}
	ui->errorLabel->setText("创建房间中...");
	ui->heightInput->setReadOnly(true);
	ui->widthInput->setReadOnly(true);
	ui->patternNumberInput->setReadOnly(true);
	ui->timeInput->setReadOnly(true);
	ui->modeButton->setEnabled(false);
	ui->levelButton->setEnabled(false);
	ui->backButton->setEnabled(false);
	ui->createRoomButton->setEnabled(false);
	emit createRoomRequest(mode + 1, width, height, patternNumber, time);
}

void CreateRoomWindow::onHeightInput()
{
	auto h = ui->heightInput->text();
	if (h.isEmpty()) {
		ui->heightInput->setText(QString::number(height));
	} else if (h.toInt() != height && setHeight(h.toInt())) { setLevel(3); }
	setFocus();
}

void CreateRoomWindow::onWidthInput()
{
	auto w = ui->widthInput->text();
	if (w.isEmpty()) {
		ui->widthInput->setText(QString::number(width));
	} else if (w.toInt() != width && setWidth(w.toInt())) { setLevel(3); }
	setFocus();
}

void CreateRoomWindow::onPatternNumberInput()
{
	auto p = ui->patternNumberInput->text();
	if (p.isEmpty()) {
		ui->patternNumberInput->setText(QString::number(patternNumber));
	} else if (p.toInt() != patternNumber && setPatternNumber(p.toInt())) { setLevel(3); }
	setFocus();
}

void CreateRoomWindow::onTimeInput()
{
	auto t = ui->timeInput->text();
	if (t.isEmpty()) {
		ui->timeInput->setText(QString::number(time));
	} else if (t.toInt() != time && setTime(t.toInt())) { setLevel(3); }
	setFocus();
}
