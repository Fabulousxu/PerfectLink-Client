#ifndef PLAYERSOCKET_H
#define PLAYERSOCKET_H
#pragma once
#include <QTcpSocket>
#include <QJsonObject>
#include <QComboBox>
#include <QTextBrowser>
#include <QTableWidget>
#include "game.h"
class Room;

class PlayerSocket : public QObject
{
    Q_OBJECT
public:
    static QTableWidget *userTable; //控件
    static QTextBrowser *stateDisplay; //控件
private:
    QTcpSocket *socket;
    quint64 id; //玩家id，0是未分配、未登录
    Room *gamingRoom; //游戏房间
    enum EState{
        OFFLINE,//未登录，注册全程应为此状态，登录前为此状态
        ONLINE,//登录但未游戏
        IN_ROOM,//在房间里但没准备
        PREPARE,//在房间里并且准备好了
        GAMING //正在游戏中
    } state; //OFFLINE->ONLINE->  IN_ROOM->PREPARE->GAMING->ONLINE->  ...->OFFLINE
public:
    /**
     * @brief PlayerSocket构造函数
     * @param socket_ QTcpSocket指针
     * @param parent 父对象
     */
    explicit PlayerSocket(QTcpSocket *socket_, QWidget *parent=nullptr);

    ~PlayerSocket();
    /**
     * @brief 设置控件
     * @param userTable_ 控件
     * @param stateDisplay_ 控件
     */
    static void setWidget(QTableWidget *userTable_, QTextBrowser *stateDisplay_);
    /**
     * @brief 将收到的信息变成Json
     * @param bytesMsg 收到的二进制信息
     * @return 一个JSON对象
     */
    static QJsonObject requestInterpreter(QByteArray bytesMsg);

    /**
     * @brief 回复给客户一条消息
     * @param replyCode 回复消息的类别代号
     * @param data 回复消息的data字段
     */
    void reply(Reply::EType replyCode, const QJsonObject &data);
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
    /**
     * @brief 玩家是否准备中
     * @return 是则true
     */
    bool isPrepare() const {return state==PREPARE;}
private:
    /**
     * @brief 注册消息响应
     * @param nickname 玩家昵称
     * @param password 玩家密码
     */
    void onRegister(const QString &nickname, const QString &password);
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
    void onLogIn(quint64 id, const QString &password);
    /**
     * @brief 创建房间响应
     * @param playerLimit 玩家数上限
     * @param height 地图高度
     * @param width 地图宽度
     * @param patternNumber 图案样数
     * @param time 游戏时长
     */
    void onCreateRoom(int playerLimit, int height, int width, int patternNumber, int time);
    /**
     * @brief 索取房间信息响应
     * @param playerLimit 房间游戏人数上限
     */
    void onRequireRooms(int playerLimit);
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
     * @brief 玩家准备响应
     */
    void onPrepare();
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
public slots:
    void onGameBegin(const QJsonObject &data);
signals:
    void move(quint64 id, Direction direction);
};

#endif // PLAYERSOCKET_H
