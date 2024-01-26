#ifndef ROOM_H
#define ROOM_H
#include "playersocket.h"
#include "game.h"
class Room : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构建一个新房间的静态函数
     * @param host 房主Socket
     * @return 构建的新房间
     */
    static Room* add(PlayerSocket *host);
    /**
     * @brief 移除一个房间的静态函数
     * @param id 移除房间的id值
     */
    static bool remove(quint64 id);

    /**
     * @brief 向房间内添加玩家
     * @param player 添加的玩家socket
     */
    void addPlayer(PlayerSocket *player);
    /**
     * @brief 移除房间内的玩家
     * @param player 移除的玩家socket
     */
    void removePlayer(PlayerSocket *player);
    /**
     * @brief 开启房间游戏
     */
    void beginGame();
    /**
     * @brief 获取房间Id字符串
     * @return 字符串，表示id
     */
    QString getIdString() const {return QString::number(id);}
    /**
     * @brief 获取房间信息
     * @return 一个JSON
     * {"roomId":"id", players:[{"id":"id", "nickname":"name"}, ...]}
     * 第一个是房主信息
     */
    QJsonObject getRoomInfo() const;
    /**
     * @brief 获取房主id
     * @return 房主id，数字
     */
    quint64 getHostId() const {return players[0]->getId();}
    /**
     * @brief 获取房间内玩家个数
     * @return 玩家个数，int
     */
    int getPlayerCount() const {return players.size();}
private:
    explicit Room(PlayerSocket *host, uint64_t id_, QObject *parent = nullptr);//不许自己创Room
    Game *game;
    QList<PlayerSocket*> players; //[0]房主
    quint64 id; //0是未分配

};

#endif // ROOM_H
