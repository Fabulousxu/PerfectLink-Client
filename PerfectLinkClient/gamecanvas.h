#pragma once

#include <qwidget.h>
#include <qpainter.h>
#include <qrandom.h>
#include <qvector.h>
#include <qmap.h>

#define WALL_WIDTH 1 /* 外圈墙格数 */
#define SURROUNDING 3 /* 外圈墙和地板格数 */
#define MATCHING 700 /* 匹配消除时间700ms */
#define MOVE_TIME 100 /* 人物移动动画时间100ms */

enum Direction { Up, Left, Down, Right };
inline Direction opposite(Direction d) { return (Direction)(d < Down ? d + 2 : d - 2); }
inline QPoint neighbor(int x, int y, Direction d) { return d & 1 ? QPoint(x + d - 2, y) : QPoint(x, y + d - 1); }
inline QPoint neighbor(const QPoint &p, Direction d) { return neighbor(p.x(), p.y(), d); }

class Cell : public QWidget {
	Q_OBJECT
public:
	Cell(int x, int y, QWidget *parent);
	Cell(QPoint p, QWidget *parent) : Cell(p.x(), p.y(), parent) {}
	static int size;
};

class Block : public Cell {
	Q_OBJECT
public:
	Block(int x, int y, int pt, QWidget *parent); /* -1是墙, 0是地板, 1-32是图案 */
	Block(QPoint p, int pt, QWidget *parent) : Block(p.x(), p.y(), pt, parent) {}
	void paintEvent(QPaintEvent *event) override;
	void setPattern(int pt);
	static void loadPicture();
private:
	int pattern;
	static QPixmap *picture;
};

class Player : public Cell {
	Q_OBJECT
public:
	Player(int x, int y, int pt, QWidget *parent);
	Player(QPoint p, int pt, QWidget *parent) : Player(p.x(), p.y(), pt, parent) {}
	QPoint getPosition() { return position; }
	void setPosition(int x, int y);
	void setPosition(QPoint p) { setPosition(p.x(), y()); }
	void paintEvent(QPaintEvent *event) override;
	static void loadPicture();
//private:
	int pattern;
	QPoint position;
	Direction direction;
	static QPixmap *picture;
};

class GameCanvas : public QWidget {
	Q_OBJECT
public:
	GameCanvas(int w, int h, QWidget *parent);
	~GameCanvas() {}
	int mapWidth() { return block.size(); }
	int mapHeight() { return block.empty() ? 0 : block[0].size(); }
	void setPattern(const QVector<QVector<int>> &p);
	void appendPlayer(quint64 id, int pt); /* 当有新的玩家进入房间时使用该方法 */
	void removePlayer(quint64 id); /* 当有玩家退出房间时使用该方法 */
	void initialPlayer(); /* 当游戏准备开始时, 使用此方法将玩家移动至初始位置 */
//private:
	QVector<QVector<Block *>> block;
	QMap<quint64, Player *> player; /* 上限4人 */
};
