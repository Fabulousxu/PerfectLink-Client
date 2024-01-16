#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindowClass())
{
    ui->setupUi(this);
    showFullScreen();

    gameWindow = new GameWindow(this);
    gameWindow->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
