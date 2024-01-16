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
### 请求匹配 request=3, reply=3
- 客户端向服务端发送
``` Json
    "data":{
        "mode":2,/*3,4,人数*/
    }
```
- 服务器随时回复（接收到request时一个，等待结束时一个（如果不是在匹配时就正好凑齐））
1. 4人没凑齐：匹配中
``` Json
    "data":{
        "state":0
    }
```
2. 4人凑齐了，匹配
``` Json
    "data":{
        "state":1,
        "roomInfo":{
            "id":"id_string",
            "map":{
                "width":10,
                "height":10,
                "blocks":[
                    [0,1,2,3,],
                    [],//...先x后y的索引
                ]
            }
        },
        "playerInfo":[
            {
                "id":"id_string",
                "nickname":"昵称"
            }
            //, ...
        ]
    }
```
可以匹配时自主退出
### 取消匹配 request=4, reply=4
- 客户端向服务器发送: data:{}
- 服务器回复：
``` Json
    "data":{
        "state":true //or false，如果已经凑齐了
    }
```

注意：服务器需要精准找到队列里的这个人剔除

### 游戏过程
#### 移动 request=5, reply=5
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
#### 方块变动 reply=6
- 服务器发送
``` Json
    "data":{
        "x":0,
        "y":0,
        "state":0//,1,2,3
    }
```
#### 道具变动 reply=7
- 服务器发送
``` Json
    "data":{
        "x":0,
        "y":0,
        "type":0,//1,2,3
        "state":0//,1,2,3
    }
```
#### 道具效果 reply=8
- 服务器群发/发送
``` Json
    "data":{
        "type":0,//1,2,3
        //TODO
    }
```
#### 分数变动 reply=9
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
#### 游戏结束 reply=10
- 服务器群发 data:{}
>## 服务器

>## 客户端