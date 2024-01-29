#include "playerinfo.h"
#include <QRandomGenerator>


// TODO: 现在用Map只是权宜之计，后面是要改用数据库维护的

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

void PlayerInfo::add(quint64 id, const QString &nickname, const QString &password) {
    PlayerInfo *pInfo = new PlayerInfo(nickname, password);
    id_player_map.insert(id, pInfo);
}

bool PlayerInfo::load() {
    QFile *file = new QFile(ACCOUNT_FILE_PATH);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) { return false; }
    accountsJson = QJsonDocument::fromJson(file->readAll()).object();
    file->close();
    QMutexLocker locker(&id_player_mutex);
    for (auto it = accountsJson.constBegin(); it != accountsJson.constEnd(); ++it) {
        auto id = it.value().toString().toULongLong();
        auto nickname = it.value().toObject().value("nickname").toString();
        auto password = it.value().toObject().value("password").toString();
    }
}
