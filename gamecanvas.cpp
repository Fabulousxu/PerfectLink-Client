#include "gamecanvas.h"

#define MATCH_TIME 700 /* 方块匹配消除显示时间700ms */

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
  } else {
    pattern = pt + 6;
  }
}

void Block::paintEvent(QPaintEvent *event) {
  QPainter *p = new QPainter(this);
  /* 找到每个图案的图片以后, 把pattern>=7的情况删掉 */
  // if (pattern < 7) {
  p->drawPixmap(0, 0, width(), height(), picture[pattern]);
  //} else {
  //  p->setPen(QPen(Qt::black, 2));
  //  p->setBrush(Qt::white);
  //  p->drawRect(0, 0, width(), height());
  //  p->drawText(0, 0, width(), height(), Qt::AlignCenter,
  //              QString::number(pattern - 6));
  //}
  delete p;
}

void Block::setPattern(int pt) {
  if (pt == -1) {
    pattern = QRandomGenerator::global()->bounded(0, 3);
  } else if (pt == 0) {
    pattern = QRandomGenerator::global()->bounded(3, 7);
  } else {
    pattern = pt + 6;
  }
  update();
}

void Block::loadPicture() {
  picture = new QPixmap[39];
  picture[0].load(":/MainWindow/image/wall1.png");
  picture[1].load(":/MainWindow/image/wall2.png");
  picture[2].load(":/MainWindow/image/wall3.png");
  picture[3].load(":/MainWindow/image/floor1.png");
  picture[4].load(":/MainWindow/image/floor2.png");
  picture[5].load(":/MainWindow/image/floor3.png");
  picture[6].load(":/MainWindow/image/floor4.png");
  for (int i = 1; i <= 32; ++i) {
    picture[i + 6].load(":/MainWindow/image/pattern" + QString::number(i) +
                        ".png");
  }
}

QPixmap *Block::picture = nullptr;

Select::Select(int x, int y, int pt, QWidget *parent)
    : Cell(x, y, parent), pattern(pt) {}

QPixmap *Select::picture = nullptr;

void Select::setPattern(int pt) {
  pattern = pt;
  update();
}

void Select::paintEvent(QPaintEvent *event) {
  QPainter *p = new QPainter(this);
  p->drawPixmap(0, 0, width(), height(), picture[pattern]);
  delete p;
}

void Select::loadPicture() {
  picture = new QPixmap[7];
  for (int i = 0; i < 7; ++i) {
    picture[i].load(":/MainWindow/image/select" + QString::number(i + 1) +
                    ".png");
  }
}

Player::Player(int x, int y, int pt, QWidget *parent) : Cell(x, y, parent) {
  pattern = pt;
  direction = Down;
  position = QPoint(x, y);
  moveTimer = new QTimer(this);
  moveTimer->setInterval(REFRESH_TIME);
}

void Player::setPosition(int x, int y) {
  position = QPoint(x, y);
  move(position * Cell::size);
}

void Player::setDirection(Direction d) {
  direction = d;
  update();
}

void Player::moveAnimation(Direction d, bool flag) {
  static double rate = (double)REFRESH_TIME / (double)MOVE_TIME;
  static int maxCount = MOVE_TIME / REFRESH_TIME;
  static int count;
  setDirection(d);
  moveTimer->disconnect();
  if (moveTimer->isActive()) {
    moveTimer->stop();
    move(this->getPosition() * Cell::size);
  }
  if (flag) {
    position = neighbor(position, d);
    connect(moveTimer, &QTimer::timeout, this, [this, d] {
      ++count;
      if (count >= maxCount) {
        move(this->getPosition() * Cell::size);
        this->moveTimer->stop();
      } else {
        move(this->pos() + directionPoint(d) * Cell::size * rate);
      }
    });
  } else {
    connect(moveTimer, &QTimer::timeout, this, [this, d] {
      ++count;
      if (count >= maxCount) {
        move(this->getPosition() * Cell::size);
        this->moveTimer->stop();
      } else if (count >= maxCount / 2) {
        move(this->pos() - directionPoint(d) * Cell::size * 0.2 * rate);
      } else {
        move(this->pos() + directionPoint(d) * Cell::size * 0.2 * rate);
      }
    });
  }
  count = 0;
  move(this->pos() + directionPoint(d) * Cell::size * rate);
  moveTimer->start();
}

void Player::paintEvent(QPaintEvent *event) {
  QPainter *p = new QPainter(this);
  p->drawPixmap(0, 0, width(), height(), picture[pattern * 4 + direction]);
  delete p;
}

void Player::loadPicture() {
  picture = new QPixmap[28];
  for (auto i = 0; i < 7; ++i) {
    picture[i * 4 + Up].load(":/MainWindow/image/player" +
                             QString::number(i + 1) + "_up.png");
    picture[i * 4 + Left].load(":/MainWindow/image/player" +
                               QString::number(i + 1) + "_left.png");
    picture[i * 4 + Down].load(":/MainWindow/image/player" +
                               QString::number(i + 1) + "_down.png");
    picture[i * 4 + Right].load(":/MainWindow/image/player" +
                                QString::number(i + 1) + "_right.png");
  }
}

QPixmap *Player::picture = nullptr;

Path::Path(int w, int h, QWidget *parent) : QWidget(parent) {
  setParent(parent);
  setFixedSize(w * Cell::size, h * Cell::size);
  move(0.5 * Cell::size, 0.5 * Cell::size);
  show();
}

void Path::paintEvent(QPaintEvent *event) {
  QPainter *p = new QPainter(this);
  p->setPen(QPen(QColor(QRandomGenerator::global()->bounded(0, 256),
                        QRandomGenerator::global()->bounded(0, 256),
                        QRandomGenerator::global()->bounded(0, 256)),
                 4));
  for (auto it = path.begin(); it != path.end() - 1; ++it) {
    p->drawLine(*it * Cell::size, *(it + 1) * Cell::size);
  }
  delete p;
}

void Path::drawPath(const QVector<QPoint> &path) {
  this->path = path;
  update();
}

GameCanvas::GameCanvas(int w, int h, QWidget *parent) : QWidget(parent) {
  block = QVector<QVector<Block *>>(
      w + SURROUNDING * 2, QVector<Block *>(h + SURROUNDING * 2, nullptr));
  select = QVector<QVector<Select *>>(
      w + SURROUNDING * 2, QVector<Select *>(h + SURROUNDING * 2, nullptr));
  setParent(parent);
  Cell::size = qMin(parent->height() * 0.8 / mapHeight(),
                    parent->width() * 0.6 / mapWidth());
  setFixedSize(Cell::size * mapWidth(), Cell::size * mapHeight());
  move((parent->width() - width()) / 2, (parent->height() - height()) / 2);
  show();
  for (auto x = 0; x < mapWidth(); ++x) {
    if (x < WALL_WIDTH || x >= mapWidth() - WALL_WIDTH) {
      for (auto y = 0; y < mapHeight(); ++y) {
        block[x][y] = new Block(x, y, -1, this);
      }
    } else {
      auto y = 0;
      for (; y < WALL_WIDTH; ++y) {
        block[x][y] = new Block(x, y, -1, this);
        block[x][mapHeight() - 1 - y] =
            new Block(x, mapHeight() - 1 - y, -1, this);
      }
      for (; y < mapHeight() - WALL_WIDTH; ++y) {
        block[x][y] = new Block(x, y, 0, this);
      }
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
  if (player.size() >= 4 || player.contains(id)) {
    return;
  }
  int x, y;
  for (bool flag = true; flag;) {
    x = QRandomGenerator::global()->bounded(WALL_WIDTH,
                                            mapWidth() - WALL_WIDTH);
    y = QRandomGenerator::global()->bounded(WALL_WIDTH,
                                            mapHeight() - WALL_WIDTH);
    flag = false;
    for (auto it = player.begin(); it != player.end(); ++it) {
      if (QPoint(x, y) == it.value()->getPosition()) {
        flag = true;
        break;
      }
    }
  }
  player.insert(id, new Player(x, y, pt, this));
}

void GameCanvas::removePlayer(quint64 id) {
  auto it = player.find(id);
  if (it != player.end()) {
    delete it.value();
    player.erase(it);
  }
}

void GameCanvas::movePlayer(quint64 id, Direction d, bool flag) {
  auto it = player.find(id);
  if (it != player.end()) {
    it.value()->moveAnimation(d, flag);
  }
}

void GameCanvas::initializePlayer(quint64 id, QPoint p) {
  auto it = player.find(id);
  if (it != player.end()) {
    it.value()->setPosition(p);
    it.value()->setDirection(Down);
  }
}

void GameCanvas::selectBlock(QPoint p, quint64 id) {
  auto &pSelect = select[p.x()][p.y()];
  if (pSelect) {
    pSelect->setPattern(player[id]->getPattern());
    pSelect->show();
  } else {
    pSelect = new Select(p, player[id]->getPattern(), this);
  }
}

void GameCanvas::unSelectBlock(QPoint p) {
  auto &pSelect = select[p.x()][p.y()];
  if (pSelect) {
    pSelect->hide();
  }
}

void GameCanvas::drawPath(const QVector<QPoint> &path) {
  Path *pathShow = new Path(mapWidth(), mapHeight(), this);
  pathShow->drawPath(path);
  auto p1 = path.front();
  auto p2 = path.back();
  QTimer::singleShot(MATCH_TIME, [pathShow, this, p1, p2] {
    delete pathShow;
    delete select[p1.x()][p1.y()];
    delete select[p2.x()][p2.y()];
    select[p1.x()][p1.y()] = nullptr;
    select[p2.x()][p2.y()] = nullptr;
    block[p1.x()][p1.y()]->setPattern(0);
    block[p2.x()][p2.y()]->setPattern(0);
  });
}
