#pragma once

#include <QtWidgets/QWidget>
#include "ui_mainwindow.h"
#include "startwindow.h"
#include "homewindow.h"
#include "gamewindow.h"
#include "socket.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() { delete ui; }

private:
    Ui::MainWindowClass *ui;
    Socket *socket;
    StartWindow *startWindow;
    HomeWindow *homeWindow;
    GameWindow *gameWindow;
    struct AccountInfomation {
        quint64 id;
        QString nickname;
    } accountInfomation; /* 账户信息 */
    quint64 roomId;

public slots:
    void onLoginSuccess(quint64 id, const QString &nickname); /* 登录成功 */
    void onLogoffSuccess(); /* 登出成功 */
    void onCreateRoomSuccess(quint64 rid); /* 创建房间成功 */
    void onEnterRoomSuccess(int playerLimit, int width, int height, int patternNumber, int time
        , const QVector<QPair<quint64, QString>> &playerInfomation); /* 进入房间成功 */
    void onExitRoomSuccess(); /* 离开房间成功 */
};
