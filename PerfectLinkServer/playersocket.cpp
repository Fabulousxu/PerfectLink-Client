#include "room.h"
#include "playerinfo.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>
namespace Reply
{
#define ErrDef(errorName, errorString) constexpr char (errorName)[]=(errorString)
ErrDef(ID_ERROR, "ID doesn\'t exist");
ErrDef(PASSWORD_ERROR, "Wrong password");
ErrDef(PASSWORD_UNSAFE, "Password Unsafe");
ErrDef(ROOM_ERROR, "Room doesn\'t exist");
ErrDef(ROOM_FULL, "Too many players in room");
ErrDef(ROOM_START, "No entering because the game has begun");
ErrDef(SYNC_ERROR, "Bad network");
#undef ErrDef
}
extern QMap<quint64, PlayerInfo*> id_player_map;
extern QMap<quint64, Room*> id_room_map;

QTableWidget *PlayerSocket::userTable=nullptr;
QTextBrowser *PlayerSocket::stateDisplay=nullptr;

PlayerSocket::PlayerSocket(QTcpSocket *socket_,QWidget *parent)
    :QObject(parent)
    ,socket(socket_)
    ,id(0)
    ,gamingRoom(nullptr)
    ,state(OFFLINE)
{
    connect(socket, &QTcpSocket::readyRead, this, &PlayerSocket::onRead);
    connect(socket, &QTcpSocket::disconnected, this, &PlayerSocket::onDisconnect);
    //记录有用户接入
    stateDisplay->insertHtml("User <b>"+socket->peerAddress().toString()+"</b> In <br>");
}
void PlayerSocket::setWidget(QTableWidget *userTable_,QTextBrowser *stateDisplay_)
{
    RUN_ONLY_ONCE()
    if(!userTable) userTable=userTable_;
    if(!stateDisplay) stateDisplay=stateDisplay_;
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
    qDebug() << "send:[[\n" << msg << "\n]]";
    socket->write(QJsonDocument(msg).toJson());
    socket->flush();
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
        userTable->item(rowCnt,3)->setText(socket->peerAddress().toString());
    }
    state=state_;
}
void PlayerSocket::onRead()
{
    auto jsonMsg=requestInterpreter(socket->readAll());

    qDebug() << "receive[[\n" << jsonMsg << "\n]]";

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
        onCreateRoom(
            data.value("playerLimit").toInt(),
            data.value("height").toInt(),
            data.value("width").toInt(),
            data.value("patternNumber").toInt(),
            data.value("time").toInt()
        );
        break;
    case Request::REQUIRE_ROOMS :
        onRequireRooms(data.value("playerLimit").toInt());
        break;
    case Request::ENTER_ROOM:
        onEnterRoom(data.value("roomId").toString().toULongLong());
        break;
    case Request::EXIT_ROOM:
        onExitRoom();
        break;
    case Request::PREPARE:
        onPrepare();
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
    //TODO 没想好2024.2.1，可能是退出登录

    disconnect(this, 0, this, 0);
    this->deleteLater();
}

void PlayerSocket::onRegister(const QString &nickname, const QString &password)
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
    if(!PlayerInfo::remove(id)) reply(Reply::LOGOFF, {
        {"state",false},
        {"error",Reply::ID_ERROR}
    });
    else reply(Reply::LOGOFF, {{"state",true}});
    this->id=0;
    setPlayerState(OFFLINE);
}

void PlayerSocket::onLogIn(quint64 id, const QString &password)
{
    if(state!=OFFLINE || this->id) return; //不要重复登录
    if (!id_player_map.contains(id)) {
        reply(Reply::LOGIN, { {"state",false},{"error",Reply::ID_ERROR} });
    }
    else if(!id_player_map.value(id)->isPasswordMatched(password))//密码错误
        reply(Reply::LOGIN, {{"state",false},{"error",Reply::PASSWORD_ERROR}});
    else{
        reply(Reply::LOGIN, {
            {"state",true},
            {"nickname",id_player_map.value(id)->getNickName()}
        });
        this->id=id;
        //setPlayerState(ONLINE);
        state = ONLINE;
    }
}

void PlayerSocket::onCreateRoom(int playerLimit, int height, int width, int patternNumber, int time)
{
    if(state!=ONLINE) return;
    gamingRoom=Room::add(this,playerLimit, height, width, patternNumber, time);
    reply(Reply::CREATE_ROOM,{{"roomId", gamingRoom->getIdString()}});
    state=IN_ROOM;
}

void PlayerSocket::onRequireRooms(int playerLimit)
{
    if(state!=ONLINE) return;
    QJsonArray array;
    auto rooms=Room::getSomeRooms(playerLimit);
    foreach(auto pRoom, rooms){
        QJsonObject roomInfo;
        roomInfo.insert("roomId",pRoom->getIdString());
        roomInfo.insert("playerCount", pRoom->getPlayerCount());
        array.append(roomInfo);
    }
    reply(Reply::REQUIRE_ROOMS, {{"roomInfo", array}});
}

void PlayerSocket::onEnterRoom(quint64 roomId)
{
    if(state!=ONLINE) return;
    if(!id_room_map.contains(roomId))//房间不存在
    {
        reply(Reply::ENTER_ROOM,{
            {"state",false},
            {"error",Reply::ROOM_ERROR}
        });
        return;
    }
    auto pRoom=id_room_map.value(roomId);
    if(pRoom->getPlayerCount()>=pRoom->getPlayerLimit())//人数太多
    {
        reply(Reply::ENTER_ROOM,{
            {"state", false},
            {"error",Reply::ROOM_FULL}
        });
        return;
    }
    //加入成功
    auto data = pRoom->getRoomInfo();
    data.insert("state", true);
    reply(Reply::ENTER_ROOM, data);
    pRoom->addPlayer(this);
    state = IN_ROOM;
    gamingRoom=pRoom;
}

void PlayerSocket::onExitRoom() //这里是申请退出房间
{
    if(state!=IN_ROOM || (!gamingRoom)) return;
    //TODO 退出失败的判断
    gamingRoom->removePlayer(this);
    reply(Reply::EXIT_ROOM, {{"state",true}});
    state=ONLINE;
    gamingRoom=nullptr;
}

void PlayerSocket::onPrepare()
{
    if(state!=IN_ROOM || (!gamingRoom)) return;
    gamingRoom->changePrepare(this, true);
    state=PREPARE;
}

void PlayerSocket::onMove(int direction)
{
    if (state!=GAMING||(!gamingRoom)) return;
    emit move(id, (Direction)direction);
}

PlayerSocket::~PlayerSocket()
{
    //if(id) PlayerInfo::remove(id);
    //TODO 没想好2024.2.1
    //if(gamingRoom) Room::remove(gamingRoom->getIdString().toInt());
}
void PlayerSocket::onGameBegin(const QJsonObject &data)
{
    if(state!=PREPARE)
    {
        reply(Reply::ERROR, {{"error", Reply::SYNC_ERROR}});
        return;
    }
    reply(Reply::BEGIN_GAME, data);
    state=GAMING;
}
