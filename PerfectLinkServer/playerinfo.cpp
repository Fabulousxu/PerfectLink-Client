#include "playerinfo.h"
#include <QRandomGenerator>

QMutex id_player_mutex;
QMap<quint64, PlayerInfo *> id_player_map;
constexpr quint64 ID_MAX = 0x1fffffffffffff;
constexpr char ACCOUNT_FILE_PATH[]="./data/account.json";

// TODO: 现在用Map只是权宜之计，后面是要改用数据库维护的

QJsonObject PlayerInfo::accountsJson={};
PlayerInfo::PlayerInfo(const QString &nickName_, const QString &password_)
    :nickName(nickName_),password(password_){}
quint64 PlayerInfo::add(const QString &nickName, const QString &password)
{
    QMutexLocker locker(&id_player_mutex);
    quint64 id=QRandomGenerator::global()->bounded(1ull, ID_MAX);
    while(id_player_map.contains(id))
        id=QRandomGenerator::global()->bounded(1ull, ID_MAX);
    PlayerInfo *pInfo=new PlayerInfo(nickName, password);
    id_player_map.insert(id,pInfo);
    QJsonObject infoJson;
    infoJson.insert("nickname", nickName);
    infoJson.insert("password", password);
    accountsJson.insert(QString::number(id), infoJson);
    QFile *file = new QFile(ACCOUNT_FILE_PATH);
    file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    file->write(QJsonDocument(accountsJson).toJson());
    file->close();
    return id;
}
bool PlayerInfo::remove(quint64 id)
{
    if(!id_player_map.contains(id)) return false;
    PlayerInfo *pInfo=id_player_map.value(id);
    id_player_map.remove(id);
    delete pInfo;
    accountsJson.remove(QString::number(id));
    QFile *file = new QFile(ACCOUNT_FILE_PATH);
    file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    file->write(QJsonDocument(accountsJson).toJson());
    file->close();
    return true;
}

bool PlayerInfo::load()
{
    RUN_ONLY_ONCE(true);
    QFile *file = new QFile(ACCOUNT_FILE_PATH);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    accountsJson = QJsonDocument::fromJson(file->readAll()).object();
    file->close();
    QMutexLocker locker(&id_player_mutex);
    for (auto it = accountsJson.constBegin(); it != accountsJson.constEnd(); ++it) {
        auto id = it.value().toString().toULongLong();
        auto nickname = it.value().toObject().value("nickname").toString();
        auto password = it.value().toObject().value("password").toString();
        if(id_player_map.contains(id)) continue;
        id_player_map.insert(id,new PlayerInfo(nickname, password));
    }
    return true;
}

