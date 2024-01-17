#ifndef PLAYERSOCKET_H
#define PLAYERSOCKET_H

#include <QTcpSocket>
#include <QJsonObject>
#include <QComboBox>
#include <QTextBrowser>
#include <QTableWidget>
class PlayerSocket : public QTcpSocket
{
    Q_OBJECT
private:
    enum EReplyCode{
        ERROR=-1,//错误
        REGISTER=0,//注册
        LOGOFF,//注销
        LOGIN,//登录
        START_MATCH,//请求匹配
        CANCEL_MATCH,//取消匹配
        MOVE,//移动
        BLOCK_CHANGE,//方块移动
        TOOL_CHANGE,//道具变动
        TOOL_EFFECT,//道具效果
        MARK //分数变动
    };
    enum EState{
        OFFLINE,//未登录
        ONLINE,//登录但未游戏
        MATCHING,//正在匹配中
        GAMING //正在游戏中
    } state;
    QTableWidget *userTable;
    QTextBrowser *stateDisplay;
public:
    PlayerSocket(QObject *parent=nullptr);
    /**
     * @brief 初始化，传入控件指针是为了方便随时加信息
     * @param userTable_ 控件
     * @param stateDisplay_ 控件
     */
    void init(QTableWidget *userTable_, QTextBrowser *stateDisplay_);
    /**
     * @brief 回复给客户一条消息
     * @param replyCode 回复消息的类别代号
     * @param data 回复消息的data字段
     */
    void reply(EReplyCode replyCode, const QJsonObject &data);
    /**
     * @brief 将收到的信息变成Json
     * @param bytesMsg 收到的二进制信息
     * @return 一个JSON对象
     */
    static QJsonObject requestInterpreter(QByteArray bytesMsg);
    //响应各种请求
    void onRegister(QString nickname, QString password);//0
    void onLogOff(QString id);//1
    void onLogIn(QString id, QString password);//2
    void onStartMatch(int mode);//3
    void onCancelMatch();//4
    void onMove(int direction);//5
private slots:
    void onRead();
    void onDisconnect();
};

#endif // PLAYERSOCKET_H
