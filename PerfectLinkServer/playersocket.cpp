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
    userTable=userTable_;
    stateDisplay=stateDisplay_;
    id=0;
    gamingRoom=nullptr;
    state=OFFLINE;
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
void PlayerSocket::setPlayerState(EState state_)
{
    if(state==state_) return;
    if(state==ONLINE&&state_==OFFLINE)//下线
    {
        for(int i=0;i<userTable->rowCount();++i)
        {
            if(userTable->item(i,0)->text().toULongLong()!=id)
                continue;
            userTable->removeRow(i);
            break;
        }
    }
    else if(state==OFFLINE&&state_==ONLINE)//登录
    {
        //账号 昵称 密码 IP
        int rowCnt=userTable->rowCount();
        userTable->insertRow(rowCnt);
        userTable->item(rowCnt,0)->setText(QString::number(id));
        auto info=id_player_map.value(id);
        userTable->item(rowCnt,1)->setText(info->getNickName());
        userTable->item(rowCnt,2)->setText(info->getPassword());
        userTable->item(rowCnt,3)->setText(this->peerAddress().toString());
    }
    state=state_;
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


void PlayerSocket::onDisconnect()//断连槽函数
{



    //TODO
    disconnect(this, 0, this, 0);
    this->deleteLater();
}

void PlayerSocket::onRegister(QString nickname, QString password)
{
    if(state!=OFFLINE || id) return; //有id就不能继续注册咯
    //TODO: 检查安全性？
    id=PlayerInfo::add(nickname, password);
    reply(Reply::REGISTER, QJsonObject{
        {"state",true},
        {"id", QString::number(id)}
    });
}

void PlayerSocket::onLogOff(quint64 id)
{
    if(state!=ONLINE) return;
    if(!PlayerInfo::remove(id)) reply(Reply::LOGOFF, QJsonObject{
        {"state",false},
        {"error","ID doesn\'t exist"}
    });
    else reply(Reply::LOGOFF, QJsonObject{{"state",true}});
    this->id=0;
    setPlayerState(OFFLINE);
}

void PlayerSocket::onLogIn(quint64 id, QString password)
{
    if(state!=OFFLINE || this->id) return; //不要重复登录
    if(!id_player_map.contains(id))
        reply(Reply::LOGIN, QJsonObject{{"state",false},{"error",Reply::ID_ERROR}});
    else if(!id_player_map.value(id)->isPasswordMatched(password))//密码错误
        reply(Reply::LOGIN, QJsonObject{{"state",false},{"error",Reply::PASSWORD_ERROR}});
    else{
        reply(Reply::LOGIN, QJsonObject{
            {"state",true},
            {"nickname",id_player_map.value(id)->getNickName()}
        });
        this->id=id;
        setPlayerState(ONLINE);
    }
}

void PlayerSocket::onCreateRoom()
{
    if(state!=ONLINE) return;
    gamingRoom=Room::add(this);
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
    pRoom->addPlayer(this);
    reply(Reply::ENTER_ROOM,{{"state",true}});
    state = IN_ROOM;
    gamingRoom=pRoom;
}

void PlayerSocket::onExitRoom() //这里是申请退出房间
{
    if(state!=IN_ROOM || (!gamingRoom)) return;
    gamingRoom->removePlayer(this);
    reply(Reply::EXIT_ROOM, {{"state",true}});
    state=ONLINE;
    gamingRoom=nullptr;
}

void PlayerSocket::onBeginGame()
{
    if(state!=IN_ROOM) return;
    else if(!gamingRoom)
        reply(Reply::BEGIN_GAME, {{"state",false},{"error","Not in room"}});//! 不应发生
    else if(gamingRoom->getHostId()!=id)
        reply(Reply::BEGIN_GAME, {{"state",false},{"error",Reply::NOT_HOST}});
    else{
        //TODO
        state=GAMING;
    }
}

void PlayerSocket::onMove(int direction)
{
    //TODO
}
