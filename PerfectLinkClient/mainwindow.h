#pragma once

#include <QtWidgets/QWidget>
#include "ui_mainwindow.h"
#include "gamewindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindowClass *ui;
    GameWindow *gameWindow;
};
