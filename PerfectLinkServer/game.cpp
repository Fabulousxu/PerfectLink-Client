#include "game.h"

#include <qdebug.h>
#include <qeventloop.h>
#include <qmutex.h>

#define SCORE_SINGLE 20  // 单次得分20
QMutex game_mutex;

Player::Player(QPoint p) : position(p) {
  score = 0;
  select = nullptr;
  moveCoolDown = 320;
  moveCoolDownTimer = new QTimer();
  moveCoolDownTimer->setInterval(moveCoolDown);
  moveCoolDownTimer->setSingleShot(true);
}
Game::Game(int height_, int width_, int patternNumber_, int time_,
           QObject *parent)
    : QObject(parent),
      block{},
      player{},
      patternNumber(patternNumber_),
      time(time_) {
  QVector<int> pattern;
  int pos = QRandomGenerator::global()->bounded(1, 33);
  for (int i = 0; i < patternNumber_; ++i) {
    pattern.append((pos + i) % 32 + 1);
  }
  initializeBlock(height_, width_, pattern);

  countdown = time;
  countdownTimer = new QTimer(this);
  countdownTimer->setInterval(1000);
  connect(countdownTimer, &QTimer::timeout, this, [this] {
    if (countdown > 0) {
      --countdown;
    } else {
      countdownTimer->stop();
      emit gameEnd(getRank());
    }
  });
}
Game::~Game() {
  for (auto p : player) {
    delete p;
  }
  player.clear();
}
void Game::initializeBlock(int h, int w, QVector<int> pattern) {
  int pos = 0, num = 0, total = h * w;
  block = QVector<QVector<int>>(w + SURROUNDING * 2,
                                QVector<int>(h + SURROUNDING * 2, 0));
  auto width = getWidth();
  auto height = getHeight();
  for (auto x = 0; x < width; ++x) {
    if (x < WALL_WIDTH || x >= width - WALL_WIDTH) {
      for (auto &b : block[x]) {
        b = -1;
      }
    } else {
      for (auto y = 0; y < WALL_WIDTH; ++y) {
        block[x][y] = block[x][height - 1 - y] = -1;
      }
      if (SURROUNDING <= x && x < width - SURROUNDING) {
        for (auto y = SURROUNDING; y < width - SURROUNDING; ++y) {
          if (num++ < total / 2) {
            block[x][y] = pattern[pos++];
            if (pos >= pattern.size()) {
              pos = 0;
            }
          } else {
            block[x][y] = block[width - 1 - x][height - 1 - y];
          }
        }
      }
    }
  }
  do {
    for (auto x = SURROUNDING; x < width - SURROUNDING; ++x) {
      for (auto y = SURROUNDING; y < height - SURROUNDING; ++y) {
        auto a = QRandomGenerator::global()->bounded(SURROUNDING,
                                                     width - SURROUNDING);
        auto b = QRandomGenerator::global()->bounded(SURROUNDING,
                                                     height - SURROUNDING);
        qSwap(block[x][y], block[a][b]);
      }
    }
  } while (false);
}

void Game::start(QList<quint64> playerIds) {
  player.insert(playerIds[0], new Player(1, 1));
  if (playerIds.size() > 1) {
    player.insert(playerIds[1], new Player(getWidth() - 2, 1));
    if (playerIds.size() > 2) {
      player.insert(playerIds[2], new Player(1, getHeight() - 2));
      if (playerIds.size() > 3) {
        player.insert(playerIds[3],
                      new Player(getWidth() - 2, getHeight() - 2));
      }
    }
  }
  QTimer::singleShot(300, [this] { countdownTimer->start(); });
}

void Game::onMove(quint64 id, Direction d) {
  // QMutexLocker locker(&game_mutex);
  Player *player = this->player[id];
  if (player->moveCoolDownTimer->isActive()) {
    return;
  }
  player->moveCoolDownTimer->start();
  qDebug() << "before move: " << player->position;
  if (isFloor(getBlock(neighbor(player->position, d)))) {
    for (auto p : this->player) {
      if (neighbor(player->position, d) == p->position) {
        emit showMovePlayer(id, false, d);
        return;
      }
    }
    player->position = neighbor(player->position, d);
    emit showMovePlayer(id, true, d);
  } else {
    emit showMovePlayer(id, false, d);
    select(id, neighbor(player->position, d));
  }
}

QVector<QPoint> Game::match(const QPoint &a, const QPoint &b) {
  QVector<QPoint> path;
  a == b || getBlock(a) != getBlock(b) || isFloor(getBlock(a)) ||
      isWall(getBlock(a)) || isRemoving(getBlock(a)) ||
      !(path = matchLine(a, b)).empty() || !(path = matchTurn(a, b)).empty() ||
      !(path = matchTurn2(a, b)).empty();
  return path;
}

QVector<QPoint> Game::matchLine(const QPoint &a, const QPoint &b) {
  QVector<QPoint> path;
  if (a.x() == b.x()) {
    if (b.y() > a.y()) {
      for (auto y = a.y() + 1; y < b.y(); ++y) {
        if (!isFloor(block[a.x()][y])) {
          return path;
        }
      }
      for (auto y = a.y(); y <= b.y(); ++y) {
        path.append(QPoint(a.x(), y));
      }
    } else {
      for (auto y = a.y() - 1; y > b.y(); --y) {
        if (!isFloor(block[a.x()][y])) {
          return path;
        }
      }
      for (auto y = a.y(); y >= b.y(); --y) {
        path.append(QPoint(a.x(), y));
      }
    }
  } else if (a.y() == b.y()) {
    if (b.x() > a.x()) {
      for (auto x = a.x() + 1; x < b.x(); ++x) {
        if (!isFloor(block[x][a.y()])) {
          return path;
        }
      }
      for (auto x = a.x(); x <= b.x(); ++x) {
        path.append(QPoint(x, a.y()));
      }
    } else {
      for (auto x = a.x() - 1; x > b.x(); --x) {
        if (!isFloor(block[x][a.y()])) {
          return path;
        }
      }
      for (auto x = a.x(); x >= b.x(); --x) {
        path.append(QPoint(x, a.y()));
      }
    }
  }
  return path;
}

QVector<QPoint> Game::matchTurn(const QPoint &a, const QPoint &b) {
  if (a.x() == b.x() || a.y() == b.y()) {
    return QVector<QPoint>();
  }
  QVector<QPoint> path1, path2;
  QPoint turn1(a.x(), b.y()), turn2(b.x(), a.y());
  if (isFloor(getBlock(turn1)) && !(path1 = matchLine(a, turn1)).empty() &&
      !(path2 = matchLine(turn1, b)).empty()) {
    path1.removeLast();
    return path1 + path2;
  }
  if (isFloor(getBlock(turn2)) && !(path1 = matchLine(a, turn2)).empty() &&
      !(path2 = matchLine(turn2, b)).empty()) {
    path1.removeLast();
    return path1 + path2;
  }
  return QVector<QPoint>();
}

QVector<QPoint> Game::matchTurn2(const QPoint &a, const QPoint &b) {
  QVector<QPoint> path, tmp;
  auto width = getWidth();
  auto height = getHeight();
  int pos[2] = {0}, flag = true, length, minLength = (width + height) * 2;
  auto leftA = a.x() - 1, rightA = a.x() + 1, upA = a.y() - 1,
       downA = a.y() + 1;
  auto leftB = b.x() - 1, rightB = b.x() + 1, upB = b.y() - 1,
       downB = b.y() + 1;
  while (leftA >= SURROUNDING - 1 && isFloor(block[leftA][a.y()])) {
    --leftA;
  }
  ++leftA;
  while (rightA <= width - SURROUNDING && isFloor(block[rightA][a.y()])) {
    ++rightA;
  }
  --rightA;
  while (upA >= SURROUNDING - 1 && isFloor(block[a.x()][upA])) {
    --upA;
  }
  ++upA;
  while (downA <= height - SURROUNDING && isFloor(block[a.x()][downA])) {
    ++downA;
  }
  --downA;
  while (leftB >= SURROUNDING - 1 && isFloor(block[leftB][b.y()])) {
    --leftB;
  }
  ++leftB;
  while (rightB <= width - SURROUNDING && isFloor(block[rightB][b.y()])) {
    ++rightB;
  }
  --rightB;
  while (upB >= SURROUNDING - 1 && isFloor(block[b.x()][upB])) {
    --upB;
  }
  ++upB;
  while (downB <= height - SURROUNDING && isFloor(block[b.x()][downB])) {
    ++downB;
  }
  --downB;
  auto left = qMax(leftA, leftB), right = qMin(rightA, rightB),
       up = qMax(upA, upB), down = qMin(downA, downB);
  for (auto x = left; x <= right; ++x, flag = true) {
    if (x == a.x() || x == b.x()) {
      continue;
    }
    if (b.y() > a.y()) {
      for (auto y = a.y() + 1; y < b.y(); ++y) {
        if (!isFloor(block[x][y])) {
          flag = false;
          break;
        }
      }
    } else {
      for (auto y = a.y() - 1; y > b.y(); --y) {
        if (!isFloor(block[x][y])) {
          flag = false;
          break;
        }
      }
    }
    if (flag &&
        (length = qAbs(a.x() - x) + qAbs(b.x() - x)) + qAbs(a.y() - b.y()) <
            minLength) {
      pos[0] = 1;
      pos[1] = x;
      minLength = length;
    }
  }
  for (auto y = up; y <= down; ++y, flag = true) {
    if (y == a.y() || y == b.y()) {
      continue;
    }
    if (b.x() > a.x()) {
      for (auto x = a.x() + 1; x < b.x(); ++x) {
        if (!isFloor(block[x][y])) {
          flag = false;
          break;
        }
      }
    } else {
      for (auto x = a.x() - 1; x > b.x(); --x) {
        if (!isFloor(block[x][y])) {
          flag = false;
          break;
        }
      }
    }
    if (flag &&
        (length = qAbs(a.y() - y) + qAbs(b.y() - y)) + qAbs(a.x() - b.x()) <
            minLength) {
      pos[0] = 2;
      pos[1] = y;
      minLength = length;
    }
  }
  if (pos[0] == 1) {
    if (pos[1] > a.x()) {
      for (auto x = a.x(); x <= pos[1]; ++x) {
        path.append(QPoint(x, a.y()));
      }
    } else {
      for (auto x = a.x(); x >= pos[1]; --x) {
        path.append(QPoint(x, a.y()));
      }
    }
    if (b.y() > a.y()) {
      for (auto y = a.y() + 1; y < b.y(); ++y) {
        path.append(QPoint(pos[1], y));
      }
    } else {
      for (auto y = a.y() - 1; y > b.y(); --y) {
        path.append(QPoint(pos[1], y));
      }
    }
    if (b.x() > pos[1]) {
      for (auto x = pos[1]; x <= b.x(); ++x) {
        path.append(QPoint(x, b.y()));
      }
    } else {
      for (auto x = pos[1]; x >= b.x(); --x) {
        path.append(QPoint(x, b.y()));
      }
    }
  } else if (pos[0] == 2) {
    if (pos[1] > a.y()) {
      for (auto y = a.y(); y <= pos[1]; ++y) {
        path.append(QPoint(a.x(), y));
      }
    } else {
      for (auto y = a.y(); y >= pos[1]; --y) {
        path.append(QPoint(a.x(), y));
      }
    }
    if (b.x() > a.x()) {
      for (auto x = a.x() + 1; x < b.x(); ++x) {
        path.append(QPoint(x, pos[1]));
      }
    } else {
      for (auto x = a.x() - 1; x > b.x(); --x) {
        path.append(QPoint(x, pos[1]));
      }
    }
    if (b.y() > pos[1]) {
      for (auto y = pos[1]; y <= b.y(); ++y) {
        path.append(QPoint(b.x(), y));
      }
    } else {
      for (auto y = pos[1]; y >= b.y(); --y) {
        path.append(QPoint(b.x(), y));
      }
    }
  }
  return path;
}

void Game::select(quint64 id, const QPoint &p) {
  // QMutexLocker locker(&game_mutex);
  if (isFloor(getBlock(p)) || isWall(getBlock(p)) || isRemoving(getBlock(p))) {
    return;
  }
  Player *player = this->player[id];
  if (player->select && *player->select == p) {
    return;
  }
  for (auto it = this->player.begin(); it != this->player.end(); ++it) {
    if (it.key() == id) {
      continue;
    }
    if ((*it)->select && *(*it)->select == p) {
      delete (*it)->select;
      (*it)->select = nullptr;
    }
  }
  emit showSelectBlock(id, p);
  ///* 以下防止粘包 */
  // QEventLoop loop;
  // QTimer::singleShot(180, [&loop] { loop.quit(); });
  // loop.exec();
  ///* *********** */
  if (player->select) {
    auto oldSelect = *player->select;
    auto path = match(oldSelect, p);
    if (path.empty()) {
      emit showUnselectBlock(*player->select);
      *player->select = p;
      return;
    } else {
      player->score += SCORE_SINGLE;
      emit showMatchPath(id, path);
      ///* 以下防止粘包 */
      // QEventLoop loop1;
      // QTimer::singleShot(180, [&loop1] { loop1.quit(); });
      // loop1.exec();
      ///* *********** */
      emit showScoreChanged(id, player->score);
      delete player->select;
      player->select = nullptr;
      QTimer::singleShot(MATCH_TIME,
                         [=] { getBlock(oldSelect) = getBlock(p) = 0; });
    }
  } else {
    player->select = new QPoint(p);
    return;
  }
}

QVector<QPair<quint64, int>> Game::getRank() {
  QVector<QPair<quint64, int>> rank;
  for (auto it = player.begin(); it != player.end(); ++it) {
    rank.append(QPair<quint64, int>(it.key(), it.value()->score));
  }
  std::sort(rank.begin(), rank.end(),
            [](QPair<quint64, int> l, QPair<quint64, int> r) {
              return l.second > r.second;
            });
  return rank;
}
