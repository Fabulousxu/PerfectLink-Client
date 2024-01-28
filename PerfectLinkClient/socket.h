#pragma once

#include <qtcpsocket.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonvalue.h>

#define SERVER_IP "255.255.255.0"
#define SERVER_PORT 8000

struct Reply {
    enum Type {
        Error = -1, /* 错误 */
        Register = 0, /* 注册 */
        Logoff, /* 登出 */
        Login, /* 登录 */
        Create_Room, /* 创建房间 */
        Require_Rooms, /* 获取房间列表 */
        Enter_Room, /* 进入房间 */
        Exit_Room, /* 离开房间 */
        Begin_Game, /* 游戏开始 */
        Player_Change, /* 房间人数变动 */
        Move, /* 玩家移动 */
        Block, /* 方块变动 */
        Path, /* 显示匹配路径 */
        Tool_Change, /* 道具变动 */
        Tool_Effect, /* 道具效果 */
        Mark, /* 分数变动 */
        End_Game /* 游戏结束 */
    };
};

struct Request {
    enum Type {
        Register = 0, /* 请求注册 */
        Logoff, /* 请求登出 */
        Login, /* 请求登录 */
        Create_Room, /* 请求创建房间 */
        Require_Rooms, /* 请求获取房间列表 */
        Enter_Room, /* 请求进入房间 */
        Exit_Room, /* 请求离开房间 */
        Begin_Game, /* 请求开始游戏 */
        Move /* 玩家请求移动 */
    };
};

class Socket : public QTcpSocket
{
	Q_OBJECT

public:
	Socket(QObject *parent);
    ~Socket() {}

    void onSignupRequest(const QString &nickname, const QString &passward);
    void onLoginRequest(quint64 id, const QString &passward);
    void onLogoffRequest(quint64 id);

    enum SocketState {
        Offline, /* 未登录, 注册全程应为此状态, 登录前为此状态 */
        Online, /* 登录但未游戏 */
        In_Room, /* 在房间里但没开始游戏 */
        Gaming /* 正在游戏中 */
    } state;

private:
    void onRead();


signals:
    void signupSuccess(quint64 id);
    void signupFail(const QString &error);
    void loginSuccess(const QString &nickname);
    void loginFail(const QString &error);
    void logoffSuccess();
    void logoffFail(const QString &error);
};
