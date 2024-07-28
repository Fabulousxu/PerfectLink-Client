#pragma once

#include <qpair.h>
#include <qregularexpression.h>
#include <qtimer.h>
#include <qvalidator.h>
#include <qvector.h>
#include <qwidget.h>

#include "ui_roominfomationwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class RoomInfomationWindowClass;
};
QT_END_NAMESPACE

class RoomInfomationWindow : public QWidget {
  Q_OBJECT

 public:
  RoomInfomationWindow(QWidget *parent = nullptr);
  ~RoomInfomationWindow() { delete ui; }

  void setMode(int m);

 private:
  Ui::RoomInfomationWindowClass *ui;
  QTimer *errorShowTimer;
  int mode; /* 0为单人模式, 1为双人对战, 2为三人对战, 3为四人对战 */
  QVector<QWidget *> roomDisplay;          /* 房间列表 */
  QVector<QLabel *> roomIdLabel;           /* 房间列表房间号 */
  QVector<QLabel *> roomPlayerNumberLabel; /* 房间当前人数 */
  QVector<QLabel *> playerNumberLabel;     /* 房间总人数 */
  QVector<QPushButton *> enterRoomButton;  /* 进入房间按钮 */

  void showError(const QString &error); /* 显示错误信息 */

 public slots:
  void onRequireRoomSuccess(const QVector<QPair<quint64, int>>
                                &roomInfomation); /* 响应获取房间信息成功 */
  void onRequireRoomFail(const QString &error); /* 响应获取房间信息失败 */
  void onEnterRoomSuccess();                    /* 响应进入房间成功 */
  void onEnterRoomFail(const QString &error); /* 响应进入房间失败 */

 private slots:
  void onModeButton();
  void onBackButton();
  void onRefreshButton();
  void onRoomIdInput();
  void onEnterRoomButton(int number);

 signals:
  void backToHome(int mode);                 /* 返回home界面 */
  void requireRoomRequest(int playerNumber); /* 申请获取房间信息 */
  void searchRoomRequest(quint64 rid);       /* 申请查找房间 */
  void enterRoomRequest(quint64 rid);
};
