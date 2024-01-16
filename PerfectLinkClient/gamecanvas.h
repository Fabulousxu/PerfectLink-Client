#pragma once

#include <QWidget>
#include <QPainter>
#include "cell.h"

class GameCanvas  : public QWidget
{
	Q_OBJECT

public:
	GameCanvas(int w, int h, QWidget *parent);
	~GameCanvas();

	void paintEvent(QPaintEvent *event) override;
};
