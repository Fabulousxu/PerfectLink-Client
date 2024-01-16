#pragma once

#include <QWidget>
#include <QPainter>

class Cell  : public QWidget
{
	Q_OBJECT

public:
	Cell(int x, int y, QWidget *parent);
	Cell(QPoint p, QWidget *parent) : Cell(p.x(), p.y(), parent) {}
	~Cell();

	void paintEvent(QPaintEvent *event) override;

	static int size;
};
