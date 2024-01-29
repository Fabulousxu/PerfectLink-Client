#include "endwindow.h"

EndWindow::EndWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::EndWindowClass())
{
	ui->setupUi(this);
}

EndWindow::~EndWindow()
{
	delete ui;
}
