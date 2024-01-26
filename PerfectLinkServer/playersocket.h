#ifndef PLAYERSOCKET_H
#define PLAYERSOCKET_H
#pragma once
#include <QTcpSocket>
#include <QJsonObject>
#include <QComboBox>
#include <QTextBrowser>
#include <QTableWidget>
class Room;
namespace Reply{
enum EType
{
    ERROR=-1,//错误
    REGISTER=0,//注册
    LOGOFF,//注销
    LOGIN,//登录
    CREATE_ROOM,//创建房间
    REQUIRE_ROOMS,//申请房间列表信息
    ENTER_ROOM,//加入房间
    EXIT_ROOM,//退出房间
    BEGIN_GAME,//房间游戏开始
    PLAYER_CHANGE,//房间人数变动
    MOVE,//移动
    BLOCK_CHANGE,//方块移动
    TOOL_CHANGE,//道具变动
    TOOL_EFFECT,//道具效果
    MARK, //分数变动
    END_GAME //游戏结束
};
#define ErrDef(errorName, errorString) constexpr char (errorName)[]=(errorString)
ErrDef(ID_ERROR, "ID doesn\'t exist");
ErrDef(PASSWORD_ERROR, "Wrong password");
ErrDef(PASSWORD_UNSAFE, "Password Unsafe");
ErrDef(ROOM_ERROR, "Room doesn\'t exist");
ErrDef(ROOM_FULL, "Too many players in room");
ErrDef(ROOM_START, "No entering because the game has begun");
ErrDef(NOT_HOST, "You're not the host");
#undef ErrDef
}
namespace Request{
enum EType
{
    REGISTER=0,
    LOGOFF,
    LOGIN,
    CREATE_ROOM,
    REQUIRE_ROOMS,
    ENTER_ROOM,
    EXIT_ROOM,
    BEGIN_GAME,
    MOVE
};
}
class PlayerSocket : public QTcpSocket
{
    Q_OBJECT
private:
    QTableWidget *userTable; //控件
    QTextBrowser *stateDisplay; //控件
    quint64 id; //玩家id，0是未分配、未登录
    Room *gamingRoom; //游戏房间
    enum EState{
        OFFLINE,//未登录，注册全程应为此状态，登录前为此状态
        ONLINE,//登录但未游戏
        IN_ROOM,//在房间里但没开始游戏
        GAMING //正在游戏中
    } state; //OFFLINE->ONLINE->  IN_ROOM->GAMING->ONLINE->  ...->OFFLINE
public:
    /**
     * @brief PlayerSocket构造函数
     * @param parent 父对象
     */
    explicit PlayerSocket(QObject *parent=nullptr);
    /**
     * @brief 初始化，传入控件指针是为了方便随时加信息
     * @param userTable_ 控件
     * @param stateDisplay_ 控件
     */
    void init(QTableWidget *userTable_, QTextBrowser *stateDisplay_);
    /**
     * @brief 回复给客户一条消息
     * @param replyCode 回复消息的类别代号
     * @param data 回复消息的data字段
     */
    void reply(Reply::EType replyCode, const QJsonObject &data);
    /**
     * @brief 将收到的信息变成Json
     * @param bytesMsg 收到的二进制信息
     * @return 一个JSON对象
     */
    static QJsonObject requestInterpreter(QByteArray bytesMsg);
    /**
     * @brief 以字符串形式获取本玩家id
     * @return id字符串
     */
    QString getIdString() const {return QString::number(id);}
    /**
     * @brief 获得玩家id数字
     * @return id数字
     */
    quint64 getId() const {return id;} //登录之后就应该是非0的id
private:
    /**
     * @brief 注册消息响应
     * @param nickname 玩家昵称
     * @param password 玩家密码
     */
    void onRegister(QString nickname, QString password);
    /**
     * @brief 注销消息响应
     * @param id 注销id
     */
    void onLogOff(quint64 id);
    /**
     * @brief 登录消息响应
     * @param id 登录玩家id
     * @param password 登录输入密码
     */
    void onLogIn(quint64 id, QString password);
    /**
     * @brief 创建房间响应
     */
    void onCreateRoom();
    /**
     * @brief 索取房间信息响应
     */
    void onRequireRooms();
    /**
     * @brief 进入房间响应
     * @param roomId 房间id
     */
    void onEnterRoom(quint64 roomId);
    /**
     * @brief 退出房间响应
     */
    void onExitRoom();
    /**
     * @brief 房主开启游戏响应
     */
    void onBeginGame();
    /**
     * @brief 玩家移动响应
     * @param direction 移动方向
     */
    void onMove(int direction);
    /**
     * @brief 玩家状态可视化
     * @param state_ 当前状态
     */
    void setPlayerState(EState state_);
private slots:
    void onRead(); //读信息
    void onDisconnect(); //链接断开
};

#endif // PLAYERSOCKET_H
