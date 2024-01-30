# Perfect Link Project
>## 信息协议
### 消息格式
1. 客户端向服务器
``` Json
{
    "request": 0, //请求类别
    "data": {
        //...
    }
}
```
2. 服务器向客户端
``` Json 
{
    "reply": 0, //回复类别
    "data": {
        //...
    }
}
```
### 注册流程, request = 0, reply = 0
- 客户端向服务端
``` Json
    "data": {
        "nickname": "昵称",
        "password": "密码"
    }
```
- 服务端向客户端
1. 注册成功
``` Json
    "data": {
        "state": true,
        "id": "id" //long long to String, 服务器分配账户id    
    }
```
2. 注册失败
``` Json
    "data": {
        "state": false,
        "error": "失败原因"
    }
```
### 登录流程 request = 1, reply = 1
- 客户端向服务端
``` Json
    "data": {
        "id": "id",
        "password": "密码"
    }
```
- 服务端向客户端
1. 登录成功
``` Json
    "data": {
        "state": true,
        "nickname": "昵称"
    }
```
2. 登录失败
``` Json
    "data":{
        "state": false,
        "error": "失败原因"
    }
```
### 登出流程 request = 2, reply = 2
- 客户端向服务端
``` Json
    "data": {
    }
```
- 服务器向客户端
1. 注销成功
``` Json
    "data": {
        "state": true
    }
```
2. 注销失败
``` Json
    "data": {
        "state": false,
        "error": "失败原因"
    }
```
### 创建房间 request = 3, reply = 3
- 客户端向服务端
``` Json
    "data": {
        "playerNumber": 2, //3, 4
        "height": 6, //from 1 to 20
        "width": 6, //from 1 to 20
        "patternNumber": 8, //from 1 to 32
        "time": 90 //from 10 to 9999
    }
```
- 服务器向客户端
``` Json
    "data": {
        "roomId": "rid" //long long to String
    }
```
### 申请房间列表 request = 4, reply = 4
- 客户端向服务器
``` Json
    "data": {
        "playerNumber": 2 //3, 4
    }
```
- 服务器向客户端
``` Json
    "data": {
        "roomsInfomation": [
            {
                "roomId": "rid",
                "playerNumber": 1 //2, 3 当前房间人数
            }
            //...
        ] //至多五组, 不发送房间人数满了的房间
    }
```
### 加入房间 request = 5, reply = 5
- 客户端向服务器
``` Json
    "data": {
        "roomId": "rid"
    }
```
- 服务器向客户端
1. 加入成功
``` Json
    "data": {
        "state": true,
        "playerInfomation": [
            {
                "id": "id",
                "nickname": "nickname"
            }
            //...
        ] //playerInfomation是房间其他人的信息
    }
```
2. 加入失败
``` Json
    "data": {
        "state": false,
        "error": "失败原因" //成员已满或者房间已销毁等等
    }
```
### 退出房间 request = 6, reply = 6
- 客户端向服务器
``` Json
    "data": {
        "roomId": "rid"
    }
```
- 服务器向客户端
1. 退出成功
``` Json
    "data": {
        "state": true, 
    }
```
2. 退出失败
``` Json
    "data": {
        "state": false,
        "error": "失败原因" //游戏已开始等等
    }
```
### 玩家准备 request = 7, reply = 7
- 客户端向服务器
``` Json
    "data": {
    }
```
- 服务器向客户端群发
``` Json
    "data": {
        "playerId": "id"
    }
```
### 房间人数变动 reply = 8
- 客户端向房间里其他人群发
1. 有人进入房间
``` Json
    "data": {
        "state": true,
        "playerId": "id",
        "nickname": "昵称"
    }
```
2. 有人离开房间
``` Json
    "data": {
        "state": true,
        "playerId": "id",
    }
```
### 游戏开始 reply = 9
- 服务器向客户端群发
``` Json
    "data": {
        "map": [
            [0, 1, 2, ]
            //...
            //先x后y的方块
        ]
    }
```
### 游戏过程
#### 移动 request = 8, reply = 10
- 客户端向服务器
``` Json
    "data":{
        "direction": 0//,1,2,3
    }
```
- 服务端向客户端群发 
``` Json
    "data": {
        "playerId": "id",
        "direction": 0,//1,2,3
        "state": true //移动后是否改变位置
    }
```
#### 选中方块 reply = 11
- 服务器向客户端群发
``` Json
    "data": {
        "x": 0,
        "y": 0,
        "playerId": "id" //方块选中者
    }
```
#### 绘制路径 reply = 12
``` Json
    "data": {
        "path": [
            {
                "x": 0,
                "y": 0
            }
            //路径上的点
        ],
        "playerId": "id" //方块消除者
    }
```
#### 分数变动 reply = 13
- 服务器向客户端群发
``` Json
    "data": {
        "playerId": "id",
        "score": 100
    }
```
#### 游戏结束 reply = 14
- 服务器向客户端群发 
``` Json
    "data": {
    }
```
### 未知错误 reply = -1
由服务器向客户端发送未知错误
``` Json
    "data":{
        "error": "错误描述"
    }
```

## 通信协议示例如下
``` C++
namespace Reply {
    enum Type {
        Signup = 0, /* 注册 */
        Login, /* 登录 */
        Logoff, /* 登出 */
        CreateRoom, /* 创建房间 */
        RequireRoom, /* 获取房间列表 */
        EnterRoom, /* 进入房间 */
        ExitRoom, /* 离开房间 */
        Prepare, /* 准备开始 */
        PlayerChange, /* 房间人数变动 */
        Begin, /* 游戏开始 */
        Move, /* 玩家移动 */
        Select, /* 方块变动 */
        Path, /* 显示匹配路径 */
        Mark, /* 分数变动 */
        End /* 游戏结束 */
    };
};

namespace Request {
    enum Type {
        Signup = 0, /* 请求注册 */
        Login, /* 请求登录 */
        Logoff, /* 请求登出 */
        CreateRoom, /* 请求创建房间 */
        RequireRoom, /* 请求获取房间列表 */
        EnterRoom, /* 请求进入房间 */
        ExitRoom, /* 请求离开房间 */
        Prepare, /* 准备开始 */
        Move /* 玩家请求移动 */
    };
};
```