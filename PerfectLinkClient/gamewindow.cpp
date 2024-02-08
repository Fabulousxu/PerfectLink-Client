#include "gamewindow.h"

GameWindow::GameWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::GameWindowClass())
	, gameCanvas(nullptr)
{
	ui->setupUi(this);
	setParent(parent);
	setFixedSize(parent->size());
	move(0, 0);
	setAttribute(Qt::WA_StyledBackground);
	setFocusPolicy(Qt::StrongFocus);
	errorShowTimer = new QTimer(this);
	errorShowTimer->setSingleShot(true);
	errorShowTimer->setInterval(3000);
	for (Direction d = Up; d <= Right; d = (Direction)(d + 1)) {
		keyTimer[d] = new QTimer(this);
		keyTimer[d]->setInterval(100);
		connect(keyTimer[d], &QTimer::timeout, this, [this, d] {
			emit moveRequest(d);
			}
		);
	}
	connect(errorShowTimer, &QTimer::timeout, this, [this] { ui->errorLabel->clear(); });
	connect(ui->exitRoomButton, &QPushButton::clicked, this, &GameWindow::onExitRoomButton);
	connect(ui->prepareButton, &QPushButton::clicked, this, &GameWindow::onPrepareButton);
	
	countdownTimer = new QTimer(this);
	countdownTimer->setInterval(20);
	connect(countdownTimer, &QTimer::timeout, this, [this] {
		if (countdown > 0) {
			countdown -= 20;
			ui->timeInnerRect->resize((ui->timeOuterRect->width() - 6) * countdown * 0.001 / time
				, ui->timeInnerRect->height());
			ui->countdownLabel->setText(QString::number(countdown / 1000));
		}
		}
	);
}

void GameWindow::setParameter(int playerLimit, int w, int h, int patternNumber, int time)
{
	mode = playerLimit - 1;
	width = w;
	height = h;
	this->patternNumber = patternNumber;
	this->time = time;
	countdown = time * 1000;
	ui->countdownLabel->setText(QString::number(time));
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat()) { return; }
	static Qt::Key key[4] = { Qt::Key_Up, Qt::Key_Left, Qt::Key_Down, Qt::Key_Right };
	for (Direction d = Up; d <= Right; d = (Direction)(d + 1)) {
		if (event->key() == key[d]) {
			emit moveRequest(d);
			if (!keyStack.empty()) { keyTimer[keyStack.top()]->stop(); }
			keyStack.push(d);
			keyTimer[d]->start();
			return;
		}
	}
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat()) { return; }
	static Qt::Key key[4] = { Qt::Key_Up, Qt::Key_Left, Qt::Key_Down, Qt::Key_Right };
	for (Direction d = Up; d <= Right; d = (Direction)(d + 1)) {
		if (event->key() == key[d]) {
			if (!keyStack.isEmpty() && keyStack.top() == d) {
				keyStack.pop();
				keyTimer[d]->stop();
				if (!keyStack.isEmpty()) {
					keyTimer[keyStack.top()]->start();
				}
				return;
			} else {
				for (auto it = keyStack.begin(); it != keyStack.end(); ++it) {
					if (*it == d) {
						keyStack.erase(it);
						return;
					}
				}
			}
			return;
		}
	}
}

void GameWindow::onEnterRoomSuccess(const QVector<QPair<quint64, QPair<QString, bool>>> &playerInfomation
	, quint64 rid, quint64 id, const QString &nickname)
{
	gameCanvas = new GameCanvas(width, height, this);
	ui->roomIdLabel->setText(QString::number(rid));
	ui->widthLabel->setText(QString::number(width));
	ui->heightLabel->setText(QString::number(height));
	ui->patternNumberLabel->setText(QString::number(patternNumber));
	ui->timeLabel->setText(QString::number(time));
	QString modeText[4] = { "单人模式", "双人对战", "三人对战", "四人对战" };
	ui->modeLabel->setText(modeText[mode]);

	ui->nickname1Label->setText(nickname);
	ui->score1Label->setText("0");
	nicknameLabel.insert(id, ui->nickname1Label);
	scoreLabel.insert(id, ui->score1Label);
	pictureLabel.insert(id, ui->player1Picture);
	gameCanvas->appendPlayer(id, 0);

	ui->player2Display->hide();
	ui->player3Display->hide();
	ui->player4Display->hide();

	if (playerInfomation.size() > 0) {
		ui->nickname2Label->setText(playerInfomation[0].second.first);
		ui->score2Label->setText("0");
		nicknameLabel.insert(playerInfomation[0].first, ui->nickname2Label);
		scoreLabel.insert(playerInfomation[0].first, ui->score2Label);
		pictureLabel.insert(playerInfomation[0].first, ui->player2Picture);
		gameCanvas->appendPlayer(playerInfomation[0].first, 1);
		prepareLabel.insert(playerInfomation[0].first, ui->prepare2Label);
		if (playerInfomation[0].second.second) {
			ui->prepare2Label->show();
		} else { ui->prepare2Label->hide(); }
		ui->prepare2Label->hide();
		ui->player2Display->show();
		playerDisplay.insert(playerInfomation[0].first, ui->player2Display);
	}

	if (playerInfomation.size() > 1) {
		ui->nickname3Label->setText(playerInfomation[1].second.first);
		ui->score3Label->setText("0");
		nicknameLabel.insert(playerInfomation[1].first, ui->nickname3Label);
		scoreLabel.insert(playerInfomation[1].first, ui->score3Label);
		pictureLabel.insert(playerInfomation[1].first, ui->player3Picture);
		gameCanvas->appendPlayer(playerInfomation[1].first, 2);
		prepareLabel.insert(playerInfomation[1].first, ui->prepare3Label);
		if (playerInfomation[0].second.second) {
			ui->prepare3Label->show();
		} else { ui->prepare3Label->hide(); }
		ui->player3Display->show();
		playerDisplay.insert(playerInfomation[1].first, ui->player3Display);
	}

	if (playerInfomation.size() > 2) {
		ui->nickname4Label->setText(playerInfomation[2].second.first);
		ui->score4Label->setText("0");
		nicknameLabel.insert(playerInfomation[2].first, ui->nickname4Label);
		scoreLabel.insert(playerInfomation[2].first, ui->score4Label);
		pictureLabel.insert(playerInfomation[2].first, ui->player4Picture);
		gameCanvas->appendPlayer(playerInfomation[2].first, 3);
		prepareLabel.insert(playerInfomation[2].first, ui->prepare4Label);
		if (playerInfomation[0].second.second) {
			ui->prepare4Label->show();
		} else { ui->prepare4Label->hide(); }
		ui->player4Display->show();
		playerDisplay.insert(playerInfomation[2].first, ui->player4Display);
	}
}

void GameWindow::onPlayerEnter(quint64 id, const QString &nickname)
{
	if (playerDisplay.size() == 0) {
		ui->nickname2Label->setText(nickname);
		ui->score2Label->setText("0");
		nicknameLabel.insert(id, ui->nickname2Label);
		scoreLabel.insert(id, ui->score2Label);
		pictureLabel.insert(id, ui->player2Picture);
		gameCanvas->appendPlayer(id, 1);
		prepareLabel.insert(id, ui->prepare2Label);
		ui->prepare2Label->hide();
		ui->player2Display->show();
		playerDisplay.insert(id, ui->player2Display);
	}else if (playerDisplay.size() == 1) {
		ui->nickname3Label->setText(nickname);
		ui->score3Label->setText("0");
		nicknameLabel.insert(id, ui->nickname3Label);
		scoreLabel.insert(id, ui->score3Label);
		pictureLabel.insert(id, ui->player3Picture);
		gameCanvas->appendPlayer(id, 2);
		prepareLabel.insert(id, ui->prepare3Label);
		ui->prepare3Label->hide();
		ui->player3Display->show();
		playerDisplay.insert(id, ui->player3Display);
	} else if (playerDisplay.size() == 2) {
		ui->nickname4Label->setText(nickname);
		ui->score4Label->setText("0");
		nicknameLabel.insert(id, ui->nickname4Label);
		scoreLabel.insert(id, ui->score4Label);
		pictureLabel.insert(id, ui->player4Picture);
		gameCanvas->appendPlayer(id, 3);
		prepareLabel.insert(id, ui->prepare4Label);
		ui->prepare4Label->hide();
		ui->player4Display->show();
		playerDisplay.insert(id, ui->player4Display);
	}
}

void GameWindow::onPlayerExit(quint64 id)
{
	scoreLabel.remove(id);
	nicknameLabel.remove(id);
	pictureLabel.remove(id);
	prepareLabel.remove(id);
	playerDisplay[id]->hide();
	playerDisplay.remove(id);
	gameCanvas->removePlayer(id);
}

void GameWindow::onPrepare(quint64 id, quint64 selfId)
{
	if (id == selfId) {
		ui->prepareButton->setText("取消准备");
		ui->prepareButton->setStyleSheet("QPushButton {\n	color: rgb(255, 255, 255);\n	background-color: rgb(117, 117, 117);\n	border: 2px solid rgb(255, 255, 255);\n	border-radius: 15px;\n	padding: 20px 40px 20px 40px;\n}\nQPushButton:hover {\n	background-color: rgb(163, 163, 163);\n}\nQPushButton:pressed {\n	background-color: rgb(58, 58, 58);\n}");
	}
}

void GameWindow::onGameBegin(const QVector<QVector<int>> &map, const QVector<QPair<quint64, QPoint>> &playerPosition)
{
	if (gameCanvas) {
		gameCanvas->setPattern(map);
		for (auto pair : playerPosition) {
			gameCanvas->initializePlayer(pair.first, pair.second);
		}
	}
	ui->prepareButton->hide();
	ui->exitRoomButton->hide();
	countdownTimer->start();
	setFocus();
}

void GameWindow::onMark(quint64 id, int score)
{
	scoreLabel[id]->setText(QString::number(score));
}

void GameWindow::onGameEnd(const QVector<QPair<quint64, int>> &rank, quint64 id)
{
	delete gameCanvas;
	gameCanvas = nullptr;
	QVector<QPair<QString, int>> gameRank;
	int self;
	for (int i = 0; i < rank.size(); ++i) {
		gameRank.append({ nicknameLabel[rank[i].first]->text(), rank[i].second });
		if (rank[i].first == id) { self = i + 1; }
	}
	emit gameEnd(gameRank, self);
}

void GameWindow::onExitRoomButton()
{
	emit exitRoomRequest();
}

void GameWindow::onPrepareButton()
{
	emit prepareRequest();
}

void GameWindow::onExitRoomSuccess()
{
	scoreLabel.clear();
	nicknameLabel.clear();
	pictureLabel.clear();
	prepareLabel.clear();
	playerDisplay.clear();
	delete gameCanvas;
	emit exitRoomSuccess();
}

void GameWindow::onExitRoomFail(const QString &error)
{
	showError(error);
}

void GameWindow::onCreateRoomSuccess(quint64 rid, quint64 id, const QString &nickname)
{
	gameCanvas = new GameCanvas(width, height, this);
	ui->roomIdLabel->setText(QString::number(rid));
	ui->widthLabel->setText(QString::number(width));
	ui->heightLabel->setText(QString::number(height));
	ui->patternNumberLabel->setText(QString::number(patternNumber));
	ui->timeLabel->setText(QString::number(time));
	QString modeText[4] = { "单人模式", "双人对战", "三人对战", "四人对战" };
	ui->modeLabel->setText(modeText[mode]);
	
	ui->nickname1Label->setText(nickname);
	ui->score1Label->setText("0");
	nicknameLabel.insert(id, ui->nickname1Label);
	scoreLabel.insert(id, ui->score1Label);
	pictureLabel.insert(id, ui->player1Picture);
	gameCanvas->appendPlayer(id, 0);

	ui->player2Display->hide();
	ui->player3Display->hide();
	ui->player4Display->hide();
}

void GameWindow::showError(const QString &error) {
	ui->errorLabel->setText(error);
	errorShowTimer->start();
}