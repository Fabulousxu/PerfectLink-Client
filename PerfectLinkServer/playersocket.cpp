#include "playersocket.h"
#include <QJsonDocument>
PlayerSocket::PlayerSocket(QObject *parent)
    :QTcpSocket(parent)
    ,state(OFFLINE)
    ,userTable(nullptr)
    ,stateDisplay(nullptr)
{}
void PlayerSocket::init(QTableWidget *userTable_, QTextBrowser *stateDisplay_)
{
    state=OFFLINE;
    userTable=userTable_;
    stateDisplay=stateDisplay_;
    connect(this, &PlayerSocket::readyRead, this, &PlayerSocket::onRead);
    connect(this, &PlayerSocket::disconnected, this, &PlayerSocket::onDisconnect);
}
QJsonObject PlayerSocket::requestInterpreter(QByteArray bytesMsg)
{
    auto doc=QJsonDocument::fromJson(bytesMsg);
    return doc.object();
}
void PlayerSocket::reply(EReplyCode replyCode, const QJsonObject &data)
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
    case REGISTER:
        onRegister(data.value("nickname").toString(),data.value("password").toString());
        break;
    case LOGOFF:
        onLogOff(data.value("id").toString());
        break;
    case LOGIN:
        onLogIn(data.value("id").toString(),data.value("password").toString());
        break;
    case START_MATCH:
        onStartMatch(data.value("mode").toInt());
        break;
    case CANCEL_MATCH:
        onCancelMatch();
        break;
    case MOVE:
        onMove(data.value("direction").toInt());
        break;
    default:
        reply(ERROR, QJsonObject({
            {"error", "Wrong request code<"+QString::number(requestCode)+">"}
        }));
    }
}

//TODO: 处理过程
void PlayerSocket::onDisconnect()//断连槽函数
{

}

void PlayerSocket::onRegister(QString nickname, QString password)
{

}

void PlayerSocket::onLogOff(QString id)
{

}

void PlayerSocket::onLogIn(QString id, QString password)
{

}

void PlayerSocket::onStartMatch(int mode)
{

}

void PlayerSocket::onCancelMatch()
{

}

void PlayerSocket::onMove(int direction)
{

}
