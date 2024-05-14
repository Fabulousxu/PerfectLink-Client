#include "playerinfo.h"

#include <QRandomGenerator>

QMutex id_player_mutex;
QMap<quint64, PlayerInfo *> id_player_map;
constexpr quint64 ID_MAX = 3000;
constexpr char ACCOUNT_FILE_PATH[] = "./data/account.json";

// TODO: 现在用Map只是权宜之计，后面是要改用数据库维护的

QJsonObject PlayerInfo::accountsJson = {};
PlayerInfo::PlayerInfo(const QString &nickName_, const QString &password_)
    : nickName(nickName_), password(password_) {}
quint64 PlayerInfo::add(const QString &nickName, const QString &password) {
  QMutexLocker locker(&id_player_mutex);

  auto id = 0ull;
  do id = QRandomGenerator::global()->bounded(1ull, ID_MAX);
  while (id_player_map.contains(id));

  qDebug() << ("注册账号" + QString::number(id));

  PlayerInfo *pInfo = new PlayerInfo(nickName, password);
  id_player_map.insert(id, pInfo);
  QJsonObject infoJson;
  infoJson.insert("nickname", nickName);
  infoJson.insert("password", password);
  accountsJson.insert(QString::number(id), infoJson);
  QFile *file = new QFile(ACCOUNT_FILE_PATH);
  file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
  file->write(QJsonDocument(accountsJson).toJson());

  qDebug() << QJsonDocument(accountsJson).toJson();

  file->close();
  return id;
}
bool PlayerInfo::remove(quint64 id) {
  if (!id_player_map.contains(id)) return false;
  PlayerInfo *pInfo = id_player_map.value(id);
  id_player_map.remove(id);
  delete pInfo;
  accountsJson.remove(QString::number(id));
  QFile *file = new QFile(ACCOUNT_FILE_PATH);
  file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
  file->write(QJsonDocument(accountsJson).toJson());
  file->close();
  return true;
}

bool PlayerInfo::load() {
  RUN_ONLY_ONCE(true);
  QFile *file = new QFile(ACCOUNT_FILE_PATH);
  if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) return false;
  accountsJson = QJsonDocument::fromJson(file->readAll()).object();
  file->close();
  for (auto it = accountsJson.constBegin(); it != accountsJson.constEnd();
       ++it) {
    auto id = it.key().toULongLong();
    auto nickname = it.value().toObject().value("nickname").toString();
    auto password = it.value().toObject().value("password").toString();
    // if(id_player_map.contains(id)) continue;
    id_player_map.insert(id, new PlayerInfo(nickname, password));
  }
  return true;
}

PasswordSecurity PlayerInfo::getPasswordSecurity(const QString &password) {
  auto len = password.size();
  if (len < 6) return PasswordSecurity::SHORT;
  if (len > 26) return PasswordSecurity::LONG;
  int hasABC = 0, hasNumber = 0, hasUnderline = 0;
  for (auto c : password) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
      hasABC = 1;
    else if (c >= '0' && c <= '9')
      hasNumber = 1;
    else if (c == '_')
      hasUnderline = 1;
    else
      return PasswordSecurity::WRONG_CHAR;
  }
  if (hasABC + hasNumber + hasUnderline < 2) return PasswordSecurity::SIMPLE;
  return PasswordSecurity::SAFE;
}
