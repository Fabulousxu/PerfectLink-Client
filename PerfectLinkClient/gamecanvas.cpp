#include "gamecanvas.h"

Cell::Cell(int x, int y, QWidget *parent) : QWidget(parent) {
	setParent(parent);
	setFixedSize(size, size);
	move(x * size, y * size);
	show();
}

int Cell::size = 0;

Block::Block(int x, int y, int pt, QWidget *parent) : Cell(x, y, parent) {
	if (pt == -1) {
		pattern = QRandomGenerator::global()->bounded(0, 3);
	} else if (pt == 0) {
		pattern = QRandomGenerator::global()->bounded(3, 7);
	} else { pattern = pt + 6; }
}

void Block::paintEvent(QPaintEvent *event) {
	QPainter *p = new QPainter(this);
	/* 找到每个图案的图片以后, 把pattern>=7的情况删掉 */
	if (pattern < 7) {
		p->drawPixmap(0, 0, width(), height(), picture[pattern]);
	} else {
		p->setPen(QPen(Qt::black, 2));
		p->setBrush(Qt::white);
		p->drawRect(0, 0, width(), height());
		p->drawText(0, 0, width(), height(), Qt::AlignCenter, QString::number(pattern - 6));
	}
	delete p;
}

void Block::setPattern(int pt) {
	if (pt == -1) {
		pattern = QRandomGenerator::global()->bounded(0, 3);
	} else if (pt == 0) {
		pattern = QRandomGenerator::global()->bounded(3, 7);
	} else { pattern = pt + 6; }
	update();
}

void Block::loadPicture() {
	picture = new QPixmap[34];
	picture[0].load(":/MainWindow/image/wall1.png");
	picture[1].load(":/MainWindow/image/wall2.png");
	picture[2].load(":/MainWindow/image/wall3.png");
	picture[3].load(":/MainWindow/image/floor1.png");
	picture[4].load(":/MainWindow/image/floor2.png");
	picture[5].load(":/MainWindow/image/floor3.png");
	picture[6].load(":/MainWindow/image/floor4.png");
}

QPixmap *Block::picture = nullptr;

Player::Player(int x, int y, int pt, QWidget *parent) : Cell(x, y, parent) {
	pattern = pt;
	direction = Down;
	position = QPoint(x, y);
}

void Player::setPosition(int x, int y) {
	position = QPoint(x, y);
	move(x * size, y * size);
}

void Player::paintEvent(QPaintEvent *event) {
	QPainter *p = new QPainter(this);
	p->drawPixmap(0, 0, width(), height(), picture[pattern * 4 + direction]);
	delete p;
}

void Player::loadPicture() {
	picture = new QPixmap[28];
	for (auto i = 0; i < 7; ++i) {
		picture[i * 4 + Up].load(":/MainWindow/image/player" + QString::number(i + 1) + "_up.png");
		picture[i * 4 + Left].load(":/MainWindow/image/player" + QString::number(i + 1) + "_left.png");
		picture[i * 4 + Down].load(":/MainWindow/image/player" + QString::number(i + 1) + "_down.png");
		picture[i * 4 + Right].load(":/MainWindow/image/player" + QString::number(i + 1) + "_right.png");
	}
}

QPixmap *Player::picture = nullptr;

GameCanvas::GameCanvas(int w, int h, QWidget *parent) : QWidget(parent) {
	block = QVector<QVector<Block *>>(w + SURROUNDING * 2, QVector<Block *>(h + SURROUNDING * 2, nullptr));
	setParent(parent);
	Cell::size = qMin(parent->height() * 0.8 / mapHeight(), parent->width() * 0.6 / mapWidth());
	setFixedSize(Cell::size * mapWidth(), Cell::size * mapHeight());
	move((parent->width() - width()) / 2, (parent->height() - height()) / 2);
	for (auto x = 0; x < mapWidth(); ++x) {
		if (x < WALL_WIDTH || x >= mapWidth() - WALL_WIDTH) {
			for (auto y = 0; y < mapHeight(); ++y) { block[x][y] = new Block(x, y, -1, this); }
		} else {
			auto y = 0;
			for (; y < WALL_WIDTH; ++y) {
				block[x][y] = new Block(x, y, -1, this);
				block[x][mapHeight() - 1 - y] = new Block(x, mapHeight() - 1 - y, -1, this);
			}
			for (; y < mapHeight() - WALL_WIDTH; ++y) { block[x][y] = new Block(x, y, 0, this); }
		}
	}
}

void GameCanvas::setPattern(const QVector<QVector<int>> &p) {
	for (auto x = 0; x < mapWidth() - SURROUNDING * 2; ++x) {
		for (auto y = 0; y < mapWidth() - SURROUNDING * 2; ++y) {
			block[x + SURROUNDING][y + SURROUNDING]->setPattern(p[x][y]);
		}
	}
}

void GameCanvas::appendPlayer(quint64 id, int pt) {
	if (player.size() >= 4 || player.contains(id)) { return; }
	int x, y;
	for (bool flag = true; flag;) {
		x = QRandomGenerator::global()->bounded(WALL_WIDTH, mapWidth() - WALL_WIDTH);
		y = QRandomGenerator::global()->bounded(WALL_WIDTH, mapHeight() - WALL_WIDTH);
		flag = false;
		for (auto it = player.begin(); it != player.end(); ++it) {
			if (QPoint(x, y) == it.value()->getPosition()) { flag = true; break; }
		}
	}
	player.insert(id, new Player(x, y, pt, this));
}

void GameCanvas::removePlayer(quint64 id) {
	if (player.contains(id)) {
		delete player[id];
		player.remove(id);
	}
}

void GameCanvas::initialPlayer() {
}

