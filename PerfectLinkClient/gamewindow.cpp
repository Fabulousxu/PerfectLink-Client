#include "gamewindow.h"

GameWindow::GameWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::GameWindowClass())
{
	ui->setupUi(this);
	setParent(parent);
	setFixedSize(parent->size());
	move(0, 0);
	setAttribute(Qt::WA_StyledBackground);
}

void GameWindow::setParameter(int playerLimit, int w, int h, int patternNumber, int time)
{
	mode = playerLimit - 1;
	width = w;
	height = h;
	this->patternNumber = patternNumber;
	this->time = time;
}

void GameWindow::onEnterRoomSuccess(const QVector<QPair<quint64, QString>> &playerInfomation
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
		ui->nickname2Label->setText(playerInfomation[0].second);
		ui->score2Label->setText("0");
		nicknameLabel.insert(playerInfomation[0].first, ui->nickname2Label);
		scoreLabel.insert(playerInfomation[0].first, ui->score2Label);
		pictureLabel.insert(playerInfomation[0].first, ui->player2Picture);
		gameCanvas->appendPlayer(playerInfomation[0].first, 1);
		ui->player2Display->show();
	}

	if (playerInfomation.size() > 1) {
		ui->nickname3Label->setText(playerInfomation[1].second);
		ui->score3Label->setText("0");
		nicknameLabel.insert(playerInfomation[1].first, ui->nickname3Label);
		scoreLabel.insert(playerInfomation[1].first, ui->score3Label);
		pictureLabel.insert(playerInfomation[1].first, ui->player3Picture);
		gameCanvas->appendPlayer(playerInfomation[1].first, 2);
		ui->player3Display->show();
	}

	if (playerInfomation.size() > 2) {
		ui->nickname4Label->setText(playerInfomation[2].second);
		ui->score4Label->setText("0");
		nicknameLabel.insert(playerInfomation[2].first, ui->nickname4Label);
		scoreLabel.insert(playerInfomation[2].first, ui->score4Label);
		pictureLabel.insert(playerInfomation[2].first, ui->player4Picture);
		gameCanvas->appendPlayer(playerInfomation[2].first, 3);
		ui->player4Display->show();
	}
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
