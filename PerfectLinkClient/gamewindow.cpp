#include "gamewindow.h"

GameWindow::GameWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::GameWindowClass())
{
	ui->setupUi(this);
	setParent(parent);
	setFixedSize(parent->size());
	move(0, 0);

	gameCanvas = new GameCanvas(6, 8, this);
	gameCanvas->show();
}

GameWindow::~GameWindow()
{
	delete ui;
}
