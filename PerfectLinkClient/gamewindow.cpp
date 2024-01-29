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

	gameCanvas = new GameCanvas(6, 6, this);

}