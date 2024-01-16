#include "gamecanvas.h"

GameCanvas::GameCanvas(int w, int h, QWidget *parent)
	: QWidget(parent)
{
	setParent(parent);
	Cell::size = qMin(parent->height() * 0.8 / h, parent->width() * 0.6 / w);
	setFixedSize(Cell::size * w, Cell::size * h);
	move((parent->width() - width()) / 2, (parent->height() - height()) / 2);

	Cell *cell = new Cell(1, 1, this);
}

GameCanvas::~GameCanvas()
{}

void GameCanvas::paintEvent(QPaintEvent * event)
{
	QPainter *p = new QPainter(this);
	p->setPen(QPen(Qt::black, 5));
	p->drawRect(0, 0, width(), height());
	delete p;
}
