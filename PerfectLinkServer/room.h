#ifndef ROOM_H
#define ROOM_H
#pragma once
#include "playersocket.h"
class Room : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构建一个新房间的静态函数
     * @param host 房主Socket
     * @param playerLimit 玩家人数上限
     * @param height 地图高度
     * @param width 地图宽度
     * @param patternNumber 匹配图案数
     * @param time 游戏时长
     * @return 构建的新房间
     */
    static Room* add(
        PlayerSocket *host,
        int playerLimit,
        int height,
        int width,
        int patternNumber,
        int time
    );
    /**
     * @brief 移除一个房间的静态函数
     * @param id 移除房间的id值
     */
    static bool remove(quint64 id);

    static QList<Room*> getSomeRooms(int playerLimit, int count=5);

    /**
     * @brief 向房间内添加玩家
     * @param player 添加的玩家socket
     */
    void addPlayer(PlayerSocket *player);
    /**
     * @brief 移除房间内的玩家
     * @param player 移除的玩家socket
     * @param needBroadcast 需要广播与否
     */
    void removePlayer(PlayerSocket *player, bool needBroadcast=true);
    /**
     * @brief 玩家准备状态改变
     * @param player 玩家
     * @param prepare 准备状态
     */
    void changePrepare(PlayerSocket *player, bool prepare);
    /**
     * @brief 获取房间Id字符串
     * @return 字符串，表示id
     */
    QString getIdString() const {return QString::number(id);}
    /**
     * @brief 获取房间内玩家个数
     * @return 玩家个数，int
     */
    int getPlayerCount() const {return player_state_map.size();}
    /**
     * @brief 获取房间玩家个数上限
     * @return 玩家个数上限，int
     */
    int getPlayerLimit() const {return playerLimit;}
    /**
     * @brief 获取房间信息
     * @return Json对象
     */
    QJsonObject getRoomInfo() const;
private:
    /**
     * @brief 构造函数
     * @param host 创建房间的玩家
     * @param id_ 分配的房间ID
     * @param playerLimit_ 玩家人数上限
     * @param height 地图高度
     * @param width 地图宽度
     * @param patternNumber 匹配图案数
     * @param time 游戏时长
     * @param parent 父对象
     * @attention 不许外面调用构造函数，统一由静态方法分配
     */
    explicit Room(
        PlayerSocket *host,
        uint64_t id_,
        int playerLimit_,
        int height,
        int width,
        int patternNumber,
        int time,
        QObject *parent = nullptr
    );

    QMap<PlayerSocket*, bool> player_state_map; //[0]房主
    quint64 id; //0是未分配
    Game *game;
    int playerLimit;
    int patternNumber;
    int time;

    /**
     * @brief 给房内玩家群发
     * @param replyCode 回复代号
     * @param data JSON里的data字段
     */
    void broadcast(Reply::EType replyCode, const QJsonObject &data) const;
signals:
    void gameBegin(const QJsonObject &data); //发送给socket的，在游戏开始时
    void tryInitGame();
private slots:
    void onTryInitGame();
};

#endif // ROOM_H
