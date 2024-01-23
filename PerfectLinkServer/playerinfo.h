#ifndef PLAYERINFO_H
#define PLAYERINFO_H
#include <QMutex>
#include <QMap>

class PlayerInfo
{
    QString nickName;
    QString password;
private:
    PlayerInfo(const QString &nickName_, const QString &password_); //外界不许直接构造
public:
    /**
     * @brief 添加玩家信息的静态方法
     * @param nickName 昵称
     * @param password 密码
     * @return 分配的玩家id
     */
    static quint64 addPlayerInfo(const QString &nickName,const QString &password);
    /**
     * @brief 移除玩家信息的静态方法
     * @param id 移除的玩家id
     */
    static void removePlayerInfo(quint64 id);
    /**
     * @brief 获取玩家昵称
     * @return 昵称(字符串)
     */
    QString getNickName() const {return nickName;}
    /**
     * @brief 验证密码是否一样
     * @param password_ 输入的密码
     * @return 一样为true
     */
    bool isPasswordMatched(const QString &password_) const {return password_==password;}
};

#endif // PLAYERINFO_H
