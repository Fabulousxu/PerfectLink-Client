#ifndef PLAYERINFO_H
#define PLAYERINFO_H
#include <QMutex>
#include <QMap>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qfile.h>
#include <qtextstream.h>

#define RUN_ONLY_ONCE(refuseRetVal) \
static bool run_only_once_helper=false;\
if(run_only_once_helper)\
return refuseRetVal;\
run_only_once_helper=true;

enum class PasswordSecurity:quint8
{
    SAFE,
    SHORT,
    LONG,
    SIMPLE,
    WRONG_CHAR
};

class PlayerInfo
{
    const QString nickName;
    const QString password;
private:
    PlayerInfo(const QString &nickName_, const QString &password_); //外界不许直接构造
    static QJsonObject accountsJson;
public:
    /**
     * @brief 添加玩家信息的静态方法
     * @param nickName 昵称
     * @param password 密码
     * @return 分配的玩家id
     */
    static quint64 add(const QString &nickName,const QString &password);
    /**
     * @brief 移除玩家信息的静态方法
     * @param id 移除的玩家id
     */
    static bool remove(quint64 id);
    /**
     * @brief 用于服务器开始运行时从json文件载入账户数据
     * @return 载入是否成功
     */
    static bool load();
    /**
     * @brief 判断客户输入密码的安全性
     * @param password 客户输入的密码
     * @return 安全性状态代码
     */
    static PasswordSecurity getPasswordSecurity(const QString &password);

    /**
     * @brief 获取玩家昵称
     * @return 昵称(字符串)
     */
    QString getNickName() const {return nickName;}
    /**
     * @brief 获取玩家密码
     * @return 密码(字符串)
     */
    QString getPassword() const {return password;}
    /**
     * @brief 验证密码是否一样
     * @param password_ 输入的密码
     * @return 一样为true
     */
    bool isPasswordMatched(const QString &password_) const {return password_==password;}
};


#if 0 
./data/account.json格式示例:

{
    "10001": {
        "nickname": "user1",
        "password": "psw1"
    },
    "10002": {
        "nickname": "user2",
        "password": "psw2"
    }
}

#endif

#endif // PLAYERINFO_H



