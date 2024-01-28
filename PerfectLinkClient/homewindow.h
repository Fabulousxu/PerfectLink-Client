#pragma once

#include <QWidget>
#include <qtimer.h>
#include "ui_homewindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class HomeWindowClass; };
QT_END_NAMESPACE

class HomeWindow : public QWidget
{
	Q_OBJECT

public:
	HomeWindow(QWidget *parent = nullptr);
	~HomeWindow();

	int state; /* 0为单人模式, 1为双人对战, 2为三人对战, 3为四人对战 */

	void setAccountMessage(quint64 id, const QString &nickname);
	void onLogoffSuccess();
	void onLogoffFail(const QString &error);

private:
	Ui::HomeWindowClass *ui;
	void onModeButton();
	QTimer *errorShowTimer;

	void showError(const QString &error);

signals:
	void logoffRequest();
	void logoffSuccess();
};
