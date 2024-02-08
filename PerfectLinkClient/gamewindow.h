#pragma once

#include <QWidget>
#include <qtimer.h>
#include <qevent.h>
#include <qstack.h>
#include "ui_gamewindow.h"
#include "gamecanvas.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameWindowClass; };
QT_END_NAMESPACE

class GameWindow : public QWidget
{
	Q_OBJECT

public:
	GameCanvas *gameCanvas;

	GameWindow(QWidget *parent = nullptr);
	~GameWindow() { delete ui; }

	void setParameter(int playerLimit, int w, int h, int patternNumber, int time);
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;

private:
	Ui::GameWindowClass *ui;
	QTimer *errorShowTimer;
	QTimer *keyTimer[4]; /* 按键定时器 */
	QStack<Direction> keyStack; /* 按键栈 */
	int mode;
	int width;
	int height;
	int patternNumber;
	int time;
	QMap<quint64, QLabel *> scoreLabel;
	QMap<quint64, QLabel *> nicknameLabel;
	QMap<quint64, QLabel *> pictureLabel;
	QMap<quint64, QLabel *> prepareLabel;
	QMap<quint64, QWidget *> playerDisplay;
	QMap<quint64, bool> prepareState;
	int countdown;
	QTimer *countdownTimer;

	void showError(const QString &error); /* 显示错误信息 */

public slots:
	void onCreateRoomSuccess(quint64 rid, quint64 id, const QString &nickname);
	void onEnterRoomSuccess(const QVector<QPair<quint64, QPair<QString, bool>>> &playerInfomation
		, quint64 rid, quint64 id, const QString &nickname);
	void onExitRoomSuccess();
	void onExitRoomFail(const QString &error);
	void onPlayerEnter(quint64 id, const QString &nickname);
	void onPlayerExit(quint64 id);
	void onPrepare(quint64 id, quint64 selfId);
	void onGameBegin(const QVector<QVector<int>> &map, const QVector<QPair<quint64, QPoint>> &playerPosition);
	void onMark(quint64 id, int score);
	void onGameEnd(const QVector<QPair<quint64, int>> &rank, quint64 id);

private slots:
	void onExitRoomButton();
	void onPrepareButton();

signals:
	void exitRoomRequest();
	void prepareRequest();
	void exitRoomSuccess();
	void moveRequest(Direction direction);
	void gameEnd(const QVector<QPair<QString, int>> &rank, int self);
};
