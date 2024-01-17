# Perfect Link Project
>## 信息协议
### 消息格式
1. 客户端向服务器：
``` Json
{
    "request":0,//Number，请求的类别
    "data":{
        //...
    }
} 
```
2. 服务器向客户端：
``` Json 
{
    "reply": 0, //Number，与回复的request一致
    "data":{
        //...
    }
}
```
### 注册流程, request=0, reply=0
- 客户端向服务端发送
``` Json
    "data":{
        "nickname":"昵称",//以后拿ID登录
        "password":"密码"
        //只能数字英文以及部分标点，建议正则
    }
```
- 服务端接收信息，回复：
1. 注册成功 
``` Json
    "data":{
        "state":true,
        "id":"id_string"//服务器分配    
    }
```
2. 注册失败
``` Json
    "data":{
        "state":false,
        "error":"失败原因"
    }
```
### 注销流程 request=1, reply=1
- 客户端向服务端发送
``` Json
    "data":{ //只有登录之后才能注销
        "id":"id_string"
    }
```
- 服务器向客户端回复
1. 注销成功
``` Json
    "data":{
        "state":true
    }
```
2. 注销失败
``` Json
    "data":{
        "state":false,
        "error":"失败原因"
    }
```
### 登录流程 request=2, reply=2
- 客户端向服务端发送
``` Json
    "data":{
        "id":"id_string",//拿ID登录
        "password":"密码"
    }
```
- 服务端接收信息，回复
1. 登录成功 
``` Json
    "data":{
        "state":true,
        "nickname":"昵称"//, ...
    }
```
2. 登录失败
``` Json
    "data":{
        "state":false,
        "error":"失败原因"
    }
```
### 房间流程
#### 创建房间 request=3, reply=3
- 客户端向服务端发送 data:{}
- 服务器回复
``` Json
    "data":{
        "roomId":"id_string",
    }
```
#### 申请房间列表 request=4, reply=4
- 客户端向服务器发送: data:{}
- 服务器回复：(4人以下的房间才被算在内)
``` Json
    "data":{
        "roomsInfo":[
            {
                "roomId":"id_string",
                "hostNickname":"房主昵称",
                "playerCount":1,//2,3
            },
            //...
        ]
    }
```
#### 加入房间 request=5, reply=5
- 客户端向服务器发送: 
``` Json
    "data":{
        "roomId":"id_string"
    }
```
- 服务器向客户端发送
``` Json
    "data":{
        "state":true,//false，如果已经满员就不会加入成功
    }
```
#### 退出房间 request=6, reply=6
- 客户端向服务器发送 data:{}
- 服务器给回复：
``` Json
    "data":{
        "state":true,//false，如果已经开始就不会退出成功
    }
```
#### 房间游戏开始 request=7, reply=7
- 客户端(限房主)向服务器发送 data:{}
- 服务器回复（给玩家们同时）
``` Json
    "data":{
        "state":true,
        "map":[
            [0,1,2,],
            //先x后y的方块
        ],
        "players":[
            {
                "id":"id_string",
                "nickname":"昵称"
            },
        ]
    }
```
或者不成功
``` Json
    "data":{
        "state":false,
        "error":"不是房主/人数不够"
    }
```
#### 房间信息更新（未开始并且人数有变动时）reply=8
服务器回复
``` Json
    "data":{
        "players":[
            {
                "id":"id_string",
                "nickname":"昵称"
            },
        ]
    }
```
### 游戏过程
#### 移动 request=8, reply=9
- 客户端向服务器发送
``` Json
    "data":{
        "direction":0//,1,2,3
    }
```
- 服务端群发: 
``` Json
    "data":{
        "playerId":"id_string",
        "direction":0,//1,2,3
        "state":true//成功，false没动
    }
```
#### 方块变动 reply=10
- 服务器发送
``` Json
    "data":{
        "x":0,
        "y":0,
        "state":0//,1,2,3
    }
```
#### 道具变动 reply=11
- 服务器发送
``` Json
    "data":{
        "x":0,
        "y":0,
        "type":0,//1,2,3
        "state":0//,1,2,3
    }
```
#### 道具效果 reply=12
- 服务器群发/发送
``` Json
    "data":{
        "type":0,//1,2,3
        //TODO
    }
```
#### 分数变动 reply=13
- 服务器群发
``` Json
    "data":{
        "scores":[
            {
                "playerId":"id_string",
                "score":100
            },
            //...
        ]
    }
```
#### 游戏结束 reply=14
- 服务器群发 data:{}
### 未知错误 reply=-1
由服务器向客户端发送未知错误，如未识别的request代号等。
``` Json
    "data":{
        "error":"错误描述"
    }
```
>## 服务器代码架构
### 可视化
### 通信处理
### 游戏处理
>## 客户端代码架构