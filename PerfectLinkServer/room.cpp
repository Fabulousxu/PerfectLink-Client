#include "room.h"
#include "playerinfo.h"
#include <QRandomGenerator>
#include <QMutex>
#include <QJsonArray>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonvalue.h>

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
    , patternNumber(patternNumber)
    , time(time)
{
    connect(this, &Room::tryInitGame, this, &Room::onTryInitGame);
    addPlayer(host);
    //TODO: 把game里的可视化的部分和控件结合在一起，用PlayerSocket::静态成员；
    

}

void Room::addPlayer(PlayerSocket *player)
{
    if(player_state_map.contains(player)) return;

    connect(this, &Room::gameBegin, player, &PlayerSocket::onGameBegin);
    connect(player, &PlayerSocket::move, game, &Game::onMove);
    //涉及到game和socket沟通
    connect(game, &Game::showMovePlayer, player, [player](quint64 id, bool flag, Direction d) {
        QJsonObject data;
        data.insert("playerId", QString::number(id));
        data.insert("direction", d);
        data.insert("state", flag);
        player->reply(Reply::MOVE, data);
        }
    );
    connect(game, &Game::showSelectBlock, player, [player](quint64 id, const QPoint &p) {
        QJsonObject data;
        data.insert("x", p.x());
        data.insert("y", p.y());
        data.insert("playerId", QString::number(id));
        player->reply(Reply::SELECT, data);
        }
    );
    connect(game, &Game::showMatchPath, player, [player](quint64 id, const QVector<QPoint> &path) {
        QJsonObject data;
        QJsonArray pointArray;
        for (auto p : path) {
            QJsonObject point;
            point.insert("x", p.x());
            point.insert("y", p.y());
            pointArray.append(point);
        }
        data.insert("path", pointArray);
        player->reply(Reply::PATH, data);
        }
    );
    connect(game, &Game::showScoreChanged, player, [player](quint64 id, int score) {
        QJsonObject data;
        data.insert("playerId", QString::number(id));
        data.insert("score", score);
        player->reply(Reply::MARK, data);
        }
    );

    auto id=player->getId();
    broadcast(Reply::PLAYER_CHANGE,{
        {"enter",true},
        {"playerId",player->getIdString()},
        {"nickname",id_player_map.value(id)->getNickName()}
    });
    player_state_map.insert(player, false);
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
QJsonObject Room::getRoomInfo() const
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
    
    QJsonObject data;
    data.insert("playerLimit", playerLimit);
    data.insert("width", game->getWidth());
    data.insert("height", game->getHeight());
    data.insert("patternNumber", patternNumber);
    data.insert("time", time);
    data.insert("playerInfo", arr);
    return data;
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

    QList<quint64> playerIdList;
    for (auto it = player_state_map.begin(); it != player_state_map.end(); ++it) {
        playerIdList.append(it.key()->getId());
    }

    QJsonObject data;
    QJsonArray xArray, playerPos;
    for (auto itx = game->getBlock().begin() + SURROUNDING; itx != game->getBlock().end() - SURROUNDING; ++itx) {
        QJsonArray yArray;
        for (auto ity = itx->begin() + SURROUNDING; ity != itx->end() - SURROUNDING; ++ity) {
            yArray.append(*ity);
        }
        xArray.append(yArray);
    }
    for (auto it = game->getPlayer().begin(); it != game->getPlayer().end(); ++it) {
        QJsonObject pos;
        pos.insert("x", it.value().position.x());
        pos.insert("y", it.value().position.x());
        pos.insert("id", QString::number(it.key()));
        playerPos.append(pos);
    }
    data.insert("map", xArray);
    data.insert("playerPos", playerPos);
    emit gameBegin(data);
}
