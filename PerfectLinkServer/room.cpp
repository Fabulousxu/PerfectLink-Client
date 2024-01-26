#include "room.h"
#include "playerinfo.h"
#include <QRandomGenerator>
#include <QMutex>
#include <QJsonArray>
QMutex id_room_mutex;
constexpr quint64 ID_MAX=9007199254740991;
QMap<quint64, Room*> id_room_map;
extern QMap<quint64, PlayerInfo*> id_player_map;
Room * Room::add(PlayerSocket *host)
{
    QMutexLocker locker(&id_room_mutex);
    quint64 id=QRandomGenerator::global()->bounded(1ull, ID_MAX);
    while(id_room_map.contains(id))
        id=QRandomGenerator::global()->bounded(1ull, ID_MAX);
    return new Room(host, id, host);
}

bool Room::remove(quint64 id)
{
    if(!id_room_map.contains(id)) return false;
    QMutexLocker locker(&id_room_mutex);
    auto pRoom=id_room_map.value(id);
    id_room_map.remove(id);
    pRoom->deleteLater();
    return true;
}

Room::Room(PlayerSocket *host, uint64_t id_, QObject *parent)
    : QObject(parent), game(nullptr), players{host}, id(id_)
{}

QJsonObject Room::getRoomInfo() const
{
    QJsonObject obj{{"roomId", QString::number(id)}};
    QJsonArray arr;
    foreach(auto playerSocket, players){
        arr.append(QJsonObject{
            {"id", playerSocket->getIdString()},
            {"nickname",id_player_map.value(playerSocket->getId())->getNickName()}
        });
    }
    obj.insert("players", arr);
    return obj;
}

void Room::addPlayer(PlayerSocket *player)
{
    if(players.contains(player)) return;
    players.append(player);
}

void Room::removePlayer(PlayerSocket *player)
{
    for(int i=0;i<players.size();++i)
        if(players.at(i)==player){
            players.remove(i);
            break;
        }
}

void Room::beginGame()
{
    game=new Game(this); //TODO
}
