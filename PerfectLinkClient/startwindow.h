#pragma once

#include <QWidget>
#include <qtimer.h>
#include <qvalidator.h>
#include <qregularexpression.h>
#include "ui_startwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StartWindowClass; };
QT_END_NAMESPACE

class StartWindow : public QWidget
{
	Q_OBJECT

public:
	StartWindow(QWidget *parent = nullptr);
	~StartWindow();

	void onSignupSuccess(quint64 id);
	void onSignupFail(const QString &error);
	void onLoginSuccess(const QString &nickname);
	void onLoginFail(const QString &error);

private:
	Ui::StartWindowClass *ui;
	bool flag; /* true为登录状态, false为注册状态 */
	QTimer *errorShowTimer; /* 错误信息展示时间计时器 */

	void onShiftButton();
	void onSubmitButton();
	void showError(const QString &error);

signals:
	void signupRequest(const QString &nickname, const QString &passward);
	void loginRequest(quint64 id, const QString &passward);
	void loginSuccess(quint64 id, const QString &nickname);
};
