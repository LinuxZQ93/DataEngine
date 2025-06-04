# DataEngine

### 一、介绍

​	DataEngine（数据引擎）是专用于电力通讯行业的框架，基于C++11设计，以json配置为基础。用户基于此框架可以开发出Modbus 104 61850等规约插件。基于框架开发的好处，是数据统一处理流转，点表灵活配置，整体也更加稳定，同时能进一步提升开发效率。



### 二、使用

​	本项目基于cmake，使用前用户应当安装cmake，若提示版本过低，请升级。目前代码仅支持linux系统，支持x86及aarch64架构，有新平台需要者，可联系作者进行适配编译。

#### 2.1、编译

在任意目录处，clone代码

```
git clone https://github.com/LinuxZQ93/DataEngine.git
```

切换至项目目录进行编译，内置了build.sh脚本辅助编译（可根据需要自我修改）

```
cd ./DataEngine
./build.sh
```

默认编译的是x86架构，若是编译aarch64架构，则用./build.sh aarch64

#### 2.2、运行

编译成功后，可切换至项目./Run目录，直接执行run.sh即可。由于程序做了证书加密处理，我会不定期更新license

程序是基于配置驱动的，客户可根据配置情况，设计相应界面进行配置的修改，电力里面常用的组态软件也大都这样的逻辑，这里暂未提供相应修改工具，需要自己手动修改

**Task**

```
{
    "name": "Task1", ---任务名称，可随意取
    "enable" : false, ---使能标志
    "param": {
        "attribute": {  ---属性配置
            "bTans": false, ---是否为转发任务
            "content": [
                {
                    "name": "温湿度", ---设备名称，保持唯一
                    "devTypeName": "温湿度", ---设备类型
                    "address": 1,  ---设备地址
                    "description": "温湿度", ---描述
                    "msgInfo": "th",  ---信息
                    "nodeModel": "th", ---设备模型
                    "nodeId": "abc123", ---唯一ID
                    "extra": {
                        "enableLog": true, ---日志使能
                        "logLimitSize" : 10, ---M单位，最大存储容量限制
                        "logLimitNum" : 5,  ---最大存储个数
                        "enablePrintFrame": true ---报文是否打印
                    }
                },
                {
                    "name": "温湿度1",
                    "devTypeName": "温湿度",
                    "address": 2,
                    "description": "温湿度",
                    "msgInfo": "th",
                    "nodeModel": "th",
                    "nodeId": "abc1234",
                    "extra": {
                        "enableLog": true,
                        "logLimitSize" : 10,
                        "logLimitNum" : 5,
                        "enablePrintFrame": true
                    }
                }
            ],
            "template": "common"
        },
        "channel": {
            "template": "RS-485",  ---485通道
            "enablePrintSource": false, ---是否打印源码
            "content": {
                "comName": 0,
                "baundRate": 7,
                "stopBits": 0,
                "parity": 0,
                "dataBits": 3
            }
        }
    }
}

{
    "name": "Task2",
    "enable" : true,
    "param": {
        "attribute": {
            "bTrans": true, ---转发任务
            "content" : [
                {
                    "name": "test104Srv",
                    "description" : "test",
                    "extra": {
                        "enableLog": true,
                        "logLimitSize" : 10,
                        "logLimitNum" : 5,
                        "enablePrintFrame": true
                    }
                }
            ]
        },
        "channel": {
            "enablePrintSource": true,
            "template": "TCPServer",
            "content": {
                "localIP": "0.0.0.0",
                "localPort": 2404,
                "remoteIP": ""   ---不填写，对端任意IP可连，填写的情况下只允许此ip来连接，若限制多个ip，可用#间隔如192.168.1.99#192.168.1.100
            }
        }
    }
}
```

程序设计是任务驱动制，每个任务会有一个线程，在此处可配置转发或采集任务。采集（将下面的串口或网络设备数据采集上来为采集）转发（将采集完的数据，通过标准协议，一般为104或61850出去）

**Store**

DevStore下的第一层目录名称为设备类型名称，对应Task.json里面的devTypeName。内含规约和点表配置。TransStrore也是同理，为转发的具体规约和点表

```
{
    "name" : "IEC104转发",
    "template" : "IEC104转发", ---模板名称，会在template目录做进一步的映射
    "param": {
        "setIFrameTime": 0,
        "callTime": 1,
        "setTime": 1,
        "checkSerial": 1,
        "measureBit": 1,
        "commAddr": 1,
        "yxStart": 1,
        "ycStart": 16385,
        "ymStart": 25601,
        "ykStart": 24586,
        "ytStart": 25089,
        "yxType": 0,
        "ykType": 0,
        "ycType": 0
    }
}
```

**Template**

此处为具体配置的解释和plug库的名称配置

```
{
    "name" : "IEC104转发", 
    "plugName" : "EngineTransIEC104",  ---插件库名称，框架会根据名称自动加载相应动态库
    "param" : [
        {
            "name" : "setIFrameTime",
            "editName" : "发送多少次I帧必须收到确认(0表示不需要)",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "callTime",
            "editName" : "总召时间",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "setTime",
            "editName" : "对时时间",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "checkSerial",
            "editName" : "检测序列号",
            "editType" : 1,
            "range" : "是#否",
            "defaultValue" : "1"
        },
        {
            "name" : "measureBit",
            "editName" : "遥测数据位",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "commAddr",
            "editName" : "公共地址",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "yxStart",
            "editName" : "遥信起始地址",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "ycStart",
            "editName" : "遥测起始地址",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "ymStart",
            "editName" : "遥脉起始地址",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "ykStart",
            "editName" : "遥控起始地址",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "ytStart",
            "editName" : "遥调起始地址",
            "editType" : 0,
            "range" : "",
            "defaultValue" : "1"
        },
        {
            "name" : "yxType",
            "editName" : "遥信类型",
            "editType" : 1,
            "range" : "单点#双点",
            "defaultValue" : "0"
        },
        {
            "name" : "ykType",
            "editName" : "遥控类型",
            "editType" : 0,
            "range" : "单点#双点",
            "defaultValue" : "0"
        },
        {
            "name" : "ycTi",
            "editName" : "遥测类型",
            "editType" : 0,
            "range" : "浮点#无品质归一化#有品质归一化#标度",
            "defaultValue" : "0"
        }
    ]
}
```

**Plug**

Plug目录为插件编写的demo，用户可参考此处代码进行业务程序的编写



### 三、结语

​	本项目可供电力行业Coder参考学习，若需要商用，或有其他开发需求，请加微信HardAndBetter或直接email沟通linuxzq93@163.com