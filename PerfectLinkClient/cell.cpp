#include "cell.h"

Cell::Cell(int x, int y, QWidget *parent)
	: QWidget(parent)
{
	setParent(parent);
	setFixedSize(size, size);
	move(x * size, y * size);
	show();
}

Cell::~Cell()
{}

void Cell::paintEvent(QPaintEvent * event)
{
	QPainter *p = new QPainter(this);
	p->setPen(QPen(Qt::black, 5));
	p->drawRect(0, 0, width(), height());
	delete p;
}

int Cell::size = 0;