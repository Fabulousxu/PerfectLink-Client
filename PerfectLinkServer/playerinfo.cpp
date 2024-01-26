#include "playerinfo.h"
#include <QRandomGenerator>
QMutex id_player_mutex;
QMap<quint64, PlayerInfo*> id_player_map;
constexpr quint64 ID_MAX=0x1fffffffffffff;

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
    return id;
}
bool PlayerInfo::remove(quint64 id)
{
    if(!id_player_map.contains(id)) return false;
    PlayerInfo *pInfo=id_player_map.value(id);
    id_player_map.remove(id);
    delete pInfo;
    return true;
}

