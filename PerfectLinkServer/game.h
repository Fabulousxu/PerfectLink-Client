#pragma once

#include <qmap.h>
#include <qobject.h>
#include <qpoint.h>
#include <qrandom.h>
#include <qstring.h>
#include <qtimer.h>
#include <qvector.h>

namespace Reply {
enum EType {
  ERROR = -1,     // 错误
  REGISTER = 0,   // 注册
  LOGOFF,         // 注销
  LOGIN,          // 登录
  CREATE_ROOM,    // 创建房间
  REQUIRE_ROOMS,  // 申请房间列表信息
  ENTER_ROOM,     // 加入房间
  EXIT_ROOM,      // 退出房间
  PREPARE,        // 玩家准备
  PLAYER_CHANGE,  // 房间人数变动
  BEGIN_GAME,     // 房间游戏开始
  MOVE,           // 移动
  SELECT,         // 方块移动
  PATH,           // 显示路径
  MARK,           // 分数变动
  END_GAME        // 游戏结束
};
}
namespace Request {
enum EType {
  REGISTER = 0,
  LOGOFF,
  LOGIN,
  CREATE_ROOM,
  REQUIRE_ROOMS,
  ENTER_ROOM,
  EXIT_ROOM,
  PREPARE,
  MOVE
};
}

#define WALL_WIDTH 1   /* 外圈墙格数 */
#define SURROUNDING 3  /* 外圈墙和地板格数, 一定要设置大于1!! */
#define MATCH_TIME 700 /* 方块匹配消除显示时间700ms */
#if 0
关于图案编号, 规定0是地板, -1是围墙, -2是正在消除的图案, 1 - 32为32个预留的图案编号
#endif
inline bool isFloor(int t) { return !t; }
inline bool isWall(int t) { return !(t + 1); }
inline bool isRemoving(int t) { return !(t + 2); }

enum Direction { Up, Left, Down, Right };
inline Direction opposite(Direction d) {
  return (Direction)(d < Down ? d + 2 : d - 2);
}
inline QPoint neighbor(int x, int y, Direction d) {
  return d & 0x1 ? QPoint(x + d - 2, y) : QPoint(x, y + d - 1);
}
inline QPoint neighbor(const QPoint &p, Direction d) {
  return neighbor(p.x(), p.y(), d);
}

class Player {
 public:
  QPoint position; /* 玩家当前坐标 */
  int score;       /* 玩家当前分数 */
  QPoint *select; /* 玩家已选择的位置, 因为可以为空, 故采用指针 */
  int moveCoolDown;          /* 移动冷却时间(ms) */
  QTimer *moveCoolDownTimer; /* 移动冷却 */

  Player(QPoint p);
  Player(int x, int y) : Player(QPoint(x, y)){};
  ~Player() { delete moveCoolDownTimer; }
};

class Game : public QObject {
  Q_OBJECT

 public:
  explicit Game(int height_, int width_, int patternNumber_, int time_,
                QObject *parent = nullptr);
  ~Game();

  int getWidth() const { return block.size(); }
  int getHeight() const { return block.empty() ? 0 : block[0].size(); }
  const QVector<QVector<int>> &getBlock() { return block; } /* 获取方块 */
  void initializeBlock(int h, int w, QVector<int> pattern); /* 初始化地图 */
  const auto &getPlayer() { return player; }

  void start(QList<quint64> playerIds);

 public slots:
  void onMove(quint64 id, Direction d); /* 人物移动 */

 private:
  QVector<QVector<int>> block; /* 游戏方块图, block[x][y]即为坐标为(x, y)的方块,
                                  涵盖整个地图包括四周墙壁 */
  QMap<quint64, Player *> player; /* 用户id-玩家表 */
  int patternNumber;
  int time;
  QTimer *countdownTimer; /* 游戏倒计时定时器 */
  int countdown;          /* 游戏倒计时 */
  int &getBlock(const QPoint &p) {
    return block[p.x()][p.y()];
  } /* 通过坐标获取方块引用 */

  QVector<QPoint> match(
      const QPoint &a,
      const QPoint &b); /* 判断两个方块是否能匹配, 返回匹配路径(不能匹配为空) */
  QVector<QPoint> matchLine(const QPoint &a, const QPoint &b); /* 直线匹配 */
  QVector<QPoint> matchTurn(const QPoint &a,
                            const QPoint &b); /* 一次转弯匹配 */
  QVector<QPoint> matchTurn2(const QPoint &a,
                             const QPoint &b); /* 二次转弯匹配 */
  void select(quint64 id, const QPoint &p); /* 某玩家选中某位置的方块 */

  QVector<QPair<quint64, int>> getRank(); /* 获取比分排名, 在游戏结束时使用 */

 signals:
  void showMovePlayer(
      quint64 id, bool flag,
      Direction d); /* 显示指定玩家移动, 给定是否改变位置以及移动方向 */
  void showSelectBlock(quint64 id, const QPoint &p); /* 显示指定方块被选中 */
  void showUnselectBlock(const QPoint &p); /* 显示指定方块取消选中 */
  void showMatchPath(quint64 id,
                     const QVector<QPoint> &path); /* 显示匹配路径 */
  void showScoreChanged(quint64 id, int score);    /* 显示分数改变 */
  void gameEnd(const QVector<QPair<quint64, int>> &rank); /* 游戏结束 */
};
