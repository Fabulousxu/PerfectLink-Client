#include "room.h"
#include "playerinfo.h"
#include <QRandomGenerator>
#include <QMutex>
#include <QJsonArray>

QMutex id_room_mutex;
constexpr quint64 ID_MAX=9007199254740991;
QMap<quint64, Room*> id_room_map;
extern QMap<quint64, PlayerInfo*> id_player_map;
Room * Room::add(
    PlayerSocket *host,
    int playerLimit,
    int height,
    int width,
    int patternNumber,
    int time)
{
    QMutexLocker locker(&id_room_mutex);
    quint64 id=0;
    do id=QRandomGenerator::global()->bounded(1ull, ID_MAX);
    while(id_room_map.contains(id));
    auto res=new Room(host, id, playerLimit, height, width, patternNumber, time, PlayerSocket::userTable);
    id_room_map.insert(id, res);
    return res;
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

QList<Room*> Room::getSomeRooms(int playerLimit, int count)
{
    QList<Room*> res;
    foreach(auto pRoom, id_room_map)
    {
        if(pRoom->playerLimit!=playerLimit || pRoom->playerLimit<=pRoom->getPlayerCount())
            continue;
        res.append(pRoom);
        if(res.size()==count)
            break;
    }
    return res;
}
Room::Room(
    PlayerSocket *host,
    quint64 id_,
    int playerLimit_,
    int height,
    int width,
    int patternNumber,
    int time,
    QObject *parent)
    : QObject(parent)
    , player_state_map{}
    , id(id_)
    , game(new Game(height, width, patternNumber, time, this))
    , playerLimit(playerLimit_)
{
    connect(this, &Room::tryInitGame, this, &Room::onTryInitGame);
    addPlayer(host);
    //TODO: 把game里的可视化的部分和控件结合在一起，用PlayerSocket::静态成员；
}

void Room::addPlayer(PlayerSocket *player)
{
    if(player_state_map.contains(player)) return;
    player_state_map.insert(player,false);
    connect(this, &Room::gameBegin, player, &PlayerSocket::onGameBegin);
    connect(player, &PlayerSocket::move, game, &Game::onMove);
    //TODO 涉及到game和socket沟通，在这写
    auto id=player->getId();
    broadcast(Reply::PLAYER_CHANGE,{
        {"enter",true},
        {"playerId",player->getIdString()},
        {"nickname",id_player_map.value(id)->getNickName()}
    });
}

void Room::removePlayer(PlayerSocket *player)
{
    foreach(auto pPlayer, player_state_map.keys()){
        if(pPlayer!=player) continue;
        player_state_map.remove(pPlayer);
        disconnect(this, 0, player, 0);
        disconnect(player, 0, game, 0);
        disconnect(game, 0, player, 0);
        broadcast(Reply::PLAYER_CHANGE,{
            {"enter",false},
            {"playerId",player->getIdString()}
        });
        break;
    }
    emit tryInitGame(); //写在这，就是尽可能多的检查能不能启动游戏，防止之前卡了
}

void Room::changePrepare(PlayerSocket *player, bool prepare)
{
    if(!player_state_map.contains(player)) return;
    player_state_map[player]=prepare;
    QTimer::singleShot(200,this,[this](){
        emit tryInitGame();
    });
}
QJsonArray Room::getPlayerInfo() const
{
    QJsonArray arr;
    foreach(auto pPlayer, player_state_map.keys())
    {
        auto id=pPlayer->getId();
        auto nickname=id_player_map.value(id)->getNickName();
        arr.append(QJsonObject{
            {"id", pPlayer->getIdString()},
            {"nickname",nickname}
        });
    }
    return arr;
}

void Room::broadcast(Reply::EType replyCode, const QJsonObject &data) const
{
    foreach(auto player, player_state_map.keys())
        player->reply(replyCode, data);
}

void Room::onTryInitGame()
{
    foreach(auto prepare, player_state_map)
        if(!prepare) return;
    //TODO initGame

    //foreach : connect(player, &PlayerSocket::move, game, &Game::onMove);
    //emit gameBegin()
}
