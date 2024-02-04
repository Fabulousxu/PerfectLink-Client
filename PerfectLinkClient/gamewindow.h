#pragma once

#include <QWidget>
#include "ui_gamewindow.h"
#include "gamecanvas.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameWindowClass; };
QT_END_NAMESPACE

class GameWindow : public QWidget
{
	Q_OBJECT

public:
	GameWindow(QWidget *parent = nullptr);
	~GameWindow() { delete ui; }

	void setParameter(int playerLimit, int w, int h, int patternNumber, int time);

private:
	Ui::GameWindowClass *ui;
	GameCanvas *gameCanvas;
	int mode;
	int width;
	int height;
	int patternNumber;
	int time;
	QMap<quint64, QLabel *> scoreLabel;
	QMap<quint64, QLabel *> nicknameLabel;
	QMap<quint64, QLabel *> pictureLabel;

public slots:
	void onCreateRoomSuccess(quint64 rid, quint64 id, const QString &nickname);
	void onEnterRoomSuccess(const QVector<QPair<quint64, QString>> &playerInfomation
		, quint64 rid, quint64 id, const QString &nickname);

private slots:

signals:

};
