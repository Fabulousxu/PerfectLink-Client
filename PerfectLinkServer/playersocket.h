#ifndef PLAYERSOCKET_H
#define PLAYERSOCKET_H

#include <QTcpSocket>
#include <QJsonObject>
#include <QComboBox>
#include <QTextBrowser>
#include <QTableWidget>
class Room;
struct Reply{
    enum EType{
        ERROR=-1,//错误
        REGISTER=0,//注册
        LOGOFF,//注销
        LOGIN,//登录
        CREATE_ROOM,//创建房间
        REQUIRE_ROOMS,//申请房间列表信息
        ENTER_ROOM,//加入房间
        EXIT_ROOM,//退出房间
        BEGIN_GAME,//房间游戏开始
        PLAYER_CHANGE,//房间人数变动
        MOVE,//移动
        BLOCK_CHANGE,//方块移动
        TOOL_CHANGE,//道具变动
        TOOL_EFFECT,//道具效果
        MARK, //分数变动
        END_GAME //游戏结束
    };
};
struct Request{
    enum EType{
        REGISTER=0,
        LOGOFF,
        LOGIN,
        CREATE_ROOM,
        REQUIRE_ROOMS,
        ENTER_ROOM,
        EXIT_ROOM,
        BEGIN_GAME,
        MOVE
    };
};

class PlayerSocket : public QTcpSocket
{
    Q_OBJECT
private:
    QTableWidget *userTable;
    QTextBrowser *stateDisplay;
    quint64 id; //0是未分配、未登录
    Room *gamingRoom;
    enum EState{
        OFFLINE,//未登录，注册全程应为此状态，登录前为此状态
        ONLINE,//登录但未游戏
        IN_ROOM,//在房间里但没开始游戏
        GAMING //正在游戏中
    } state; //OFFLINE->ONLINE->  IN_ROOM->GAMING->ONLINE->  ...->OFFLINE
public:
    explicit PlayerSocket(QObject *parent=nullptr);
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
    void reply(Reply::EType replyCode, const QJsonObject &data);
    /**
     * @brief 将收到的信息变成Json
     * @param bytesMsg 收到的二进制信息
     * @return 一个JSON对象
     */
    static QJsonObject requestInterpreter(QByteArray bytesMsg);

    QString getIdString() const {return QString::number(id);}
    quint64 getId() const {return id;} //登录之后就应该是非0的id
    //响应各种请求
    void onRegister(QString nickname, QString password);
    void onLogOff(quint64 id);
    void onLogIn(quint64 id, QString password);
    void onCreateRoom();
    void onRequireRooms();
    void onEnterRoom(quint64 roomId);
    void onExitRoom();
    void onBeginGame();
    void onMove(int direction);
private slots:
    void onRead();
    void onDisconnect();
};

#endif // PLAYERSOCKET_H
