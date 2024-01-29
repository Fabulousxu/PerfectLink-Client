#pragma once

#include <QWidget>
#include "ui_gamewindow.h"
#include "gamecanvas.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameWindowClass; };
QT_END_NAMESPACE

class GameWindow : public QWidget
{
	Q_OBJECT

public:
	GameWindow(QWidget *parent = nullptr);
	~GameWindow() { delete ui; }

private:
	Ui::GameWindowClass *ui;
	GameCanvas *gameCanvas;
};
