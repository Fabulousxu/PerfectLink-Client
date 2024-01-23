#ifndef ROOM_H
#define ROOM_H
#include "playersocket.h"
#include "game.h"
class Room : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构建一个新房间的共有函数
     * @param host 房主Socket
     * @return 构建的新房间
     */
    static Room* addRoom(PlayerSocket *host);

    static void removeRoom(quint64 id);


    void addPlayer(PlayerSocket *player);
    void removePlayer(PlayerSocket *player);
    void beginGame();

    QString getIdString() const {return QString::number(id);}
    QJsonObject getRoomInfo() const;
    quint64 getHostId() const {return players[0]->getId();}
    int getPlayerCount() const {return players.size();}
private:
    explicit Room(PlayerSocket *host, uint64_t id_, QObject *parent = nullptr);//不许自己创Room
    Game *game;
    QList<PlayerSocket*> players; //[0]房主
    quint64 id; //0是未分配

};

#endif // ROOM_H
