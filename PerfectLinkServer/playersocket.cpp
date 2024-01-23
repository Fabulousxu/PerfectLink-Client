#include "playersocket.h"
#include "room.h"
#include "playerinfo.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>

extern QMap<quint64, PlayerInfo*> id_player_map;
PlayerSocket::PlayerSocket(QObject *parent)
    :QTcpSocket(parent)
    ,userTable(nullptr)
    ,stateDisplay(nullptr)
    ,id(0)
    ,gamingRoom(nullptr)
    ,state(OFFLINE)
{}
void PlayerSocket::init(QTableWidget *userTable_, QTextBrowser *stateDisplay_)
{
    state=OFFLINE;
    userTable=userTable_;
    stateDisplay=stateDisplay_;
    connect(this, &PlayerSocket::readyRead, this, &PlayerSocket::onRead);
    connect(this, &PlayerSocket::disconnected, this, &PlayerSocket::onDisconnect);
    //记录有用户接入
    stateDisplay->insertHtml("User <b>"+peerAddress().toString()+"</b> In <br>");
}
QJsonObject PlayerSocket::requestInterpreter(QByteArray bytesMsg)
{
    auto doc=QJsonDocument::fromJson(bytesMsg);
    return doc.object();
}
void PlayerSocket::reply(Reply::EType replyCode, const QJsonObject &data)
{
    QJsonObject msg({
        {"reply",replyCode},
        {"data",data},
    });
    write(QJsonDocument(msg).toJson());
}

void PlayerSocket::onRead()
{
    auto jsonMsg=requestInterpreter(this->readAll());
    int requestCode=jsonMsg.value("request").toInt();
    QJsonObject data=jsonMsg.value("data").toObject({});
    switch(requestCode){
    case Request::REGISTER:
        onRegister(data.value("nickname").toString(),data.value("password").toString());
        break;
    case Request::LOGOFF:
        onLogOff(data.value("id").toString().toULongLong());
        break;
    case Request::LOGIN:
        onLogIn(data.value("id").toString().toULongLong(),data.value("password").toString());
        break;
    case Request::CREATE_ROOM :
        onCreateRoom();
        break;
    case Request::REQUIRE_ROOMS :
        onRequireRooms();
        break;
    case Request::ENTER_ROOM:
        onEnterRoom(data.value("roomId").toString().toULongLong());
        break;
    case Request::EXIT_ROOM:
        onExitRoom();
        break;
    case Request::BEGIN_GAME:
        onBeginGame();
        break;
    case Request::MOVE:
        onMove(data.value("direction").toInt());
        break;
    default:
        reply(Reply::ERROR, QJsonObject({
            {"error", "Wrong request code<"+QString::number(requestCode)+">"}
        }));
    }
}

//TODO: 处理Request的过程
void PlayerSocket::onDisconnect()//断连槽函数
{

}

void PlayerSocket::onRegister(QString nickname, QString password)
{
//这里要分配ID，用playerinfo.h里的方法来进行操作
}

void PlayerSocket::onLogOff(quint64 id)
{

}

void PlayerSocket::onLogIn(quint64 id, QString password)
{

}

void PlayerSocket::onCreateRoom()
{
    if(state!=ONLINE) return;
    this->gamingRoom=Room::addRoom(this);
    reply(Reply::CREATE_ROOM,{{"roomId", gamingRoom->getIdString()}});
    state=IN_ROOM;
}

extern QMap<quint64, Room*> id_room_map;
void PlayerSocket::onRequireRooms()
{
    if(state!=ONLINE) return;
    QJsonArray array;
    foreach(auto pRoom, id_room_map)
    {
        auto r=pRoom->getRoomInfo();
        QJsonObject roomInfo{};
        roomInfo.insert("roomId",r.value("roomId"));
        auto &&roomPlayers=r.value("players").toArray();
        auto hostNickname=roomPlayers.at(0).toObject().value("nickname").toString();
        roomInfo.insert("hostNickname",hostNickname);
        roomInfo.insert("playerCount", roomPlayers.size());
        array.append(roomInfo);
    }
    reply(Reply::REQUIRE_ROOMS, {{"roomsInfo", array}});
}

void PlayerSocket::onEnterRoom(quint64 roomId)
{
    if(state!=ONLINE) return;
    if(!id_room_map.contains(roomId))//房间不存在
    {
        reply(Reply::ENTER_ROOM,{
            {"state",false},
            {"error","Room doesn\'t exist"}
        });
        return;
    }
    auto pRoom=id_room_map.value(roomId);
    if(pRoom->getPlayerCount()>=Game::PLAYER_COUNT_MAX)//人数太多
    {
        reply(Reply::ENTER_ROOM,{
            {"state", false},
            {"error","Too many players"}
        });
        return;
    }
    //加入成功
    gamingRoom=pRoom;
    reply(Reply::ENTER_ROOM,{{"state",true}});
    pRoom->addPlayer(this);
    state = IN_ROOM;
}

void PlayerSocket::onExitRoom()
{
    if(state!=IN_ROOM) return;




    state=ONLINE;
    gamingRoom=nullptr;
}

void PlayerSocket::onBeginGame()
{
    if(state!=IN_ROOM) return;
    if(gamingRoom->getHostId()!=id) return;
}

void PlayerSocket::onMove(int direction)
{

}
