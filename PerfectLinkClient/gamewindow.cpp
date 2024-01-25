#include "gamewindow.h"

GameWindow::GameWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::GameWindowClass())
{
	ui->setupUi(this);
	setParent(parent);
	setFixedSize(parent->size());
	move(0, 0);

	/* ртоб╡Бйтсц */
	gameCanvas = new GameCanvas(4, 4, this);
	//QVector<QVector<int>> b(4, QVector<int>(4, 0));
	//for (auto x = 0; x < 4; ++x) {
	//	for (auto y = 0; y < 4; ++y) {
	//		b[x][y] = x + y + 1;
	//	}
	//}
	//gameCanvas->setPattern(b);
	gameCanvas->appendPlayer(1, 0);
	gameCanvas->appendPlayer(2, 1);
	gameCanvas->appendPlayer(3, 2);
	gameCanvas->appendPlayer(4, 3);
	gameCanvas->player[1]->direction = Up;
	gameCanvas->player[2]->direction = Down;
	gameCanvas->player[3]->direction = Left;
	gameCanvas->player[4]->direction = Right;
	gameCanvas->show();
}

GameWindow::~GameWindow()
{
	delete ui;
}

void GameWindow::paintEvent(QPaintEvent *event) {
	QPainter *p = new QPainter(this);
	p->setBrush(Qt::black);
	p->setPen(Qt::NoPen);
	p->drawRect(0, 0, width(), height());
	delete p;
}
