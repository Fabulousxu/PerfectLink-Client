#pragma once

#include <qwidget.h>
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
	~StartWindow() { delete ui; }

private:
	Ui::StartWindowClass *ui;
	bool flag; /* true为登录状态, false为注册状态 */
	QTimer *errorShowTimer; /* 错误信息展示时间计时器 */

	void showError(const QString &error); /* 展示错误信息 */

public slots:
	void onSignupSuccess(quint64 id); /* 响应注册成功 */
	void onSignupFail(const QString &error); /* 响应注册失败 */
	void onLoginSuccess(const QString &nickname); /* 响应登录成功 */
	void onLoginFail(const QString &error); /* 响应登录失败 */

private slots:
	void onShiftButton(); /* 响应切换按钮 */
	void onSubmitButton(); /* 响应提交按钮 */

signals:
	void signupRequest(const QString &nickname, const QString &password); /* 请求注册 */
	void loginRequest(quint64 id, const QString &password); /* 请求登录 */

	void loginSuccess(quint64 id, const QString &nickname); /* 登录成功 */
};
