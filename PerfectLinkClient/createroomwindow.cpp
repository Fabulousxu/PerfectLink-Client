#include "createroomwindow.h"

CreateRoomWindow::CreateRoomWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CreateRoomWindowClass())
{
	ui->setupUi(this);
}

CreateRoomWindow::~CreateRoomWindow()
{
	delete ui;
}
