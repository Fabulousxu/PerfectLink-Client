#pragma once

#include <qtcpsocket.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonvalue.h>

#define SERVER_IP "255.255.255.0"
#define SERVER_PORT 8000

namespace Reply {
    enum Type {
        Signup = 0, /* 注册 */
        Login, /* 登录 */
        Logoff, /* 登出 */
        CreateRoom, /* 创建房间 */
        RequireRoom, /* 获取房间列表 */
        EnterRoom, /* 进入房间 */
        ExitRoom, /* 离开房间 */
        PlayerChange, /* 房间人数变动 */
        BeginGame, /* 游戏开始 */
        Move, /* 玩家移动 */
        Block, /* 方块变动 */
        Path, /* 显示匹配路径 */
        ToolChange, /* 道具变动 */
        ToolEffect, /* 道具效果 */
        Mark, /* 分数变动 */
        EndGame /* 游戏结束 */
    };
};

namespace Request {
    enum Type {
        Signup = 0, /* 请求注册 */
        Login, /* 请求登录 */
        Logoff, /* 请求登出 */
        CreateRoom, /* 请求创建房间 */
        RequireRoom, /* 请求获取房间列表 */
        EnterRoom, /* 请求进入房间 */
        ExitRoom, /* 请求离开房间 */
        BeginGame, /* 请求开始游戏 */
        Move /* 玩家请求移动 */
    };
};

class Socket : public QTcpSocket
{
	Q_OBJECT

public:
	Socket(QObject *parent);

private:


public slots:
    void onSignupRequest(const QString &nickname, const QString &password); /* 响应注册请求 */
    void onLoginRequest(quint64 id, const QString &password); /* 响应登录请求 */
    void onLogoffRequest(quint64 id); /* 响应登出请求 */

private slots:
    void onRead(); /* 处理服务器消息 */

signals:
    void signupSuccess(quint64 id); /* 注册成功 */
    void signupFail(const QString &error); /* 注册失败 */
    void loginSuccess(const QString &nickname); /* 登录成功 */
    void loginFail(const QString &error); /* 登录失败 */
    void logoffSuccess(); /* 登出成功 */
    void logoffFail(const QString &error); /* 登出失败 */
};
