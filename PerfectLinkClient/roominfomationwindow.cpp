#include "roominfomationwindow.h"

RoomInfomationWindow::RoomInfomationWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::RoomInfomationWindowClass())
{
	ui->setupUi(this);
}

RoomInfomationWindow::~RoomInfomationWindow()
{
	delete ui;
}
