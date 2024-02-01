#pragma once

#include <qwidget.h>
#include <qtimer.h>
#include <qvalidator.h>
#include <qregularexpression.h>
#include "ui_createroomwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateRoomWindowClass; };
QT_END_NAMESPACE

#define MIN_WIDTH 1
#define MAX_WIDTH 20
#define MIN_HEIGHT 1
#define MAX_HEIGHT 20
#define MIN_PATTERN 1
#define MAX_PATTERN 32
#define MIN_TIME 10
#define MAX_TIME 9999

class CreateRoomWindow : public QWidget
{
	Q_OBJECT

public:
	CreateRoomWindow(QWidget *parent = nullptr);
	~CreateRoomWindow() { delete ui; }

	void setMode(int m);
	void setLevel(int l);

private:
	Ui::CreateRoomWindowClass *ui;
	QTimer *errorShowTimer;
	int mode; /* 0为单人模式, 1为双人对战, 2为三人对战, 3为四人对战 */
	int level; /* 0为简单难度, 1为中等难度, 2为困难难度, 4为自定义 */
	int width;
	int height;
	int patternNumber;
	int time;

	void showError(const QString &error); /* 显示错误 */
	bool setWidth(int w); /* 设置横向图案数量 */
	bool setHeight(int h); /* 设置纵向图案数量*/
	bool setPatternNumber(int p); /* 设置图案种类数量 */
	bool setTime(int t); /* 设置游戏时间 */

public slots:
	void onCreateRoomSuccess(quint64 rid); /* 响应创建房间成功 */
	void onCreateRoomFail(const QString &); /* 响应创建房间失败 */

private slots:
	void onModeButton(); /* 响应模式切换按钮 */
	void onBackButton(); /* 响应返回按钮 */
	void onLevelButton(); /* 响应预设按钮 */
	void onCreateRoomButton(); /* 响应创建房间按钮 */
	void onHeightInput(); /* 响应纵向图案数量输入 */
	void onWidthInput(); /* 响应横向图案数量输入 */
	void onPatternNumberInput(); /* 响应图案种类数量输入 */
	void onTimeInput(); /* 响应游戏时间输入 */

signals:
	void backToHome(int mode); /* 返回home界面 */
	void createRoomRequest(int playerNumber, int w, int h, int patternNumber, int time); /* 请求创建房间 */
	void createRoomSuccess(quint64 rid); /* 创建房间成功 */
};
