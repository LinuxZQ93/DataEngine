#ifndef __DATAENGINE_IDATAENGINE_H__
#define __DATAENGINE_IDATAENGINE_H__

#include "Component/IUnknown.h"
#include "Component/ComponentMacroDef.h"
#include <string>
#include <mutex>
#include "json/json.h"
#include "Data/ValueWrap.h"
#include "Function/Function.h"

namespace engine {

enum FILE_TYPE
{
    F_DIR = 0,
    F_REG,
    F_UNKNOWN
};

enum CfgType {
    ATTRIBUTE,
    CHANNEL,
    POINTTABLE,
    PROTOCOL,
};

enum DATA_TYPE {
    SIGNAL,
    MEASURE,
    PULSE,
    CONTROL,
    PARAM,
    UNKNOWN,
};

enum FIX_FRAME_VALUE {
    INVALID,
    COMPLETE,
    INCOMPLETE,
};

enum CTRL_STATUS {
    OFF,
    ON,
};

enum CONNECT_STATUS {
    CONNECT,
    DISCONNECT,
};

/**
 * @brief 连接回调函数
 * void 返回值
 * CONNECT_STATUS 连接状态
 * int socket fd
 */
using connectFunc = TFunction<void, CONNECT_STATUS, int>;
/**
 * @brief 消息处理回调函数
 * void 返回值
 * int socket fd
 * const char * 接收的报文信息
 * int 报文长度
 */
using msgFunc = TFunction<void, int, const char*, int>;

class IDataEngine : public base::IUnknown {
SIMPLE_DEF_I(DataEngine, "DataEngine")

public:
    virtual bool init() { return true; }
    virtual bool start() { return true; }
    virtual bool stop() { return true; }
    virtual bool destroy() { return true; }

public:
    /**
     * @brief 获取设备的配置信息
     *
     * @param name 设备名称
     * @param type 配置类型
     * @param cfg 配置信息
     * @return 返回值
     */
    virtual bool getDevicecfg(const std::string &name, CfgType type, Json::Value &cfg) = 0;
    /**
     * @brief 获取转发配置信息
     *
     * @param name 转发名称
     * @param type 配置类型
     * @param cfg 配置信息
     * @return 返回值
     */
    virtual bool getTranscfg(const std::string &name, CfgType type, Json::Value &cfg) = 0;
    /**
     * @brief 注册连接状态函数
     *
     * @param name 设备或转发名称
     * @param func 回调函数
     * @return true 成功
     * @return false 失败
     */
    virtual bool attachConnectStatusFunc(const std::string &name, const connectFunc &func) = 0;
    /**
     * @brief 注册消息回调函数，这个是异步调用，注册后，recvFrame函数不可用，消息与发送异步处理。仅用于tcpServer
     * 多客户端的情况下
     *
     * @param name 转发或设备名称
     * @param func 消息回调
     * @return true 成功
     * @return false 失败
     */
    virtual bool attachMsgAsyncFunc(const std::string &name, const msgFunc &func) = 0;
    /**
     * @brief 关闭连接，一般仅使用与tcpServer，因为存在多个连接
     *
     * @param name 转发或设备名称
     * @param fd 描述符
     * @return true 成功
     * @return false 失败
     */
    virtual bool closeLink(const std::string &name, int fd = -1) = 0;
    /**
     * @brief 发送报文
     *
     * @param name 设备名称
     * @param sendBuf 发送内容
     * @return 返回值
     */
    virtual bool sendFrame(const std::string &name, const std::string &sendBuf) = 0;
    /**
     * @brief 精确某个通道发送函数，一般tcp server使用，因为存在多个连接，其余单连接不需要调用
     *
     * @param fd 通道描述符
     * @param name 转发或设备名称
     * @param sendBuf 发送缓冲区
     * @return true 成功
     * @return false 失败
     */
    virtual bool sendFrame(int fd, const std::string &name, const std::string &sendBuf) = 0;
    /**
     * @brief 接受报文
     *
     * @param name 设备名称
     * @param recvBuf 接受内容
     * @param timeOut 超时时间（ms）
     * @return 返回值
     */
    virtual bool recvFrame(const std::string &name, std::string &recvBuf, int timeOut) = 0;
    /**
     * @brief 同步发送报文并接受返回报文，用于不仅数据处理的协议，还有遥控遥调协议的设备。
     *
     * @param name 设备名称
     * @param mutex 锁
     * @param sendBuf 发送缓冲区
     * @param recvBuf 接受报文
     * @return true 成功
     * @return false 失败
     */
    virtual bool sendSyncRecv(const std::string &name, std::mutex *mutex, DATA_TYPE type, const std::string &sendBuf, std::string &recvBuf, int timeOut) = 0;
    /**
     * @brief 打印发送报文信息
     *
     * @param name 设备名称
     * @param frameBuf 内容
     */
    virtual void printSendFrame(const std::string &name, DATA_TYPE type, const std::string &frameBuf) = 0;
    /**
     * @brief 打印发送报文信息
     *
     * @param name 设备名称
     * @param strType 内容名称
     * @param frameBuf 内容
     */
    virtual void printSendFrame(const std::string &name, const std::string &strType, const std::string &frameBuf) = 0;
    /**
     * @brief 打印接受报文信息
     *
     * @param name 设备名称
     * @param frameBuf 内容
     */
    virtual void printRecvFrame(const std::string &name, DATA_TYPE type, const std::string &frameBuf) = 0;
    /**
     * @brief 打印接受报文信息
     *
     * @param name 设备名称
     * @param strType 内容名称
     * @param frameBuf 内容
     */
    virtual void printRecvFrame(const std::string &name, const std::string &strType, const std::string &frameBuf) = 0;
    /**
     * @brief 更新设备点值
     *
     * @param name 设备名称
     * @param type 数据类型
     * @param pointId 点号
     * @param data 数据内容
     * @return 返回值
     */
    virtual bool updateValue(const std::string &name, DATA_TYPE type, int pointId, const base::ValueWrap &data) = 0;
    /**
     * @brief 更新多个设备点值
     *
     * @param name 设备名称
     * @param type 数据类型
     * @param pointId 点号
     * @param vecData 数据内容
     * @return 返回值
     */
    virtual bool updateMultiValue(const std::string &name, DATA_TYPE type, int pointId, const std::vector<base::ValueWrap> &vecData) = 0;
    /**
     * @brief 更新设备点值（条件更新）
     *
     * @param name 设备名称
     * @param type 数据类型
     * @param condition 条件
     * @param data 数据内容
     * @return 返回值
     */
    virtual bool updateValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const base::ValueWrap &data) = 0;
    /**
     * @brief 更新多个设备点值（条件更新）
     *
     * @param name 设备名称
     * @param type 数据类型
     * @param condition 条件
     * @param vecData 值
     * @return 返回值
     */
    virtual bool updateMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const std::vector<base::ValueWrap> &vecData) = 0;
    /**
     * @brief 获取设备点的值
     *
     * @param name 设备名称
     * @param type 数据类型
     * @param pointId 点号
     * @param data 返回数据
     * @return 返回值
     */
    virtual bool getValue(const std::string &name, DATA_TYPE type, int pointId, base::ValueWrap &data) = 0;
    /**
     * @brief 获取多个设备点的值
     *
     * @param name 设备名称
     * @param type 数据类型
     * @param pointId 点号
     * @param num 数目
     * @param vecDataNew 返回的数据
     * @return 返回值
     */
    virtual bool getMultiValue(const std::string &name, DATA_TYPE type, int pointId, int num, std::vector<base::ValueWrap> &vecDataNew) = 0;
    /**
     * @brief 获取设备点值（条件获取）
     *
     * @param name 设备名称
     * @param type 数据类型
     * @param condition 条件
     * @param data 返回数据
     * @return 返回值
     */
    virtual bool getValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, base::ValueWrap &data) = 0;
    /**
     * @brief 获取多个设备点值（条件获取）
     *
     * @param name 设备名称
     * @param type 数据类型
     * @param condition 条件
     * @param num 数目
     * @param vecDataNew 返回数据
     * @return 返回值
     */
    virtual bool getMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, int num, std::vector<base::ValueWrap> &vecDataNew) = 0;
    /**
     * @brief 转发获取点值
     *
     * @param name 转发名称
     * @param type 数据类型
     * @param transId 转发点号
     * @param data 返回数据
     * @return 返回值
     */
    virtual bool getTransValue(const std::string &name, DATA_TYPE type, int transId, base::ValueWrap &data) = 0;
    /**
     * @brief 获取多个转发点值
     *
     * @param name 转发名称
     * @param type 数据类型
     * @param transId 转发点号
     * @param num 数目
     * @param data 返回数据
     * @return 返回值
     */
    virtual bool getTransMultiValue(const std::string &name, DATA_TYPE type, int transId, int num, std::vector<base::ValueWrap> &data) = 0;
    /**
     * @brief 控制命令（选择）由转发任务调用
     *
     * @param name 转发名称
     * @param transId 转发点号
     * @param status 控制状态
     * @return 返回值
     */
    virtual bool controlSelect(const std::string &name, int transId, CTRL_STATUS status) = 0;
    /**
     * @brief 控制命令（执行）由转发任务调用
     *
     * @param name 转发名称
     * @param transId 转发点号
     * @param status 控制状态
     * @return 返回值
     */
    virtual bool controlExecute(const std::string &name, int transId, CTRL_STATUS status) = 0;
    /**
     * @brief 控制命令（取消）由转发任务调用
     *
     * @param name 转发名称
     * @param transId 转发点号
     * @param status 控制状态
     * @return 返回值
     */
    virtual bool controlCancel(const std::string &name, int transId, CTRL_STATUS status) = 0;
    /**
     * @brief 遥调命令（选择）由转发任务调用
     *
     * @param name 转发名称
     * @param transId 转发点号
     * @param data 下发值
     * @return 返回值
     */
    virtual bool paramSelect(const std::string &name, int transId, double data) = 0;
    /**
     * @brief 遥调命令（执行）由转发任务调用
     *
     * @param name 转发名称
     * @param transId 转发点号
     * @param data 下发值
     * @return 返回值
     */
    virtual bool paramExecute(const std::string &name, int transId, double data) = 0;
    /**
     * @brief 遥调命令（取消）由转发任务调用
     *
     * @param name 转发名称
     * @param transId 转发点号
     * @param data 下发值
     * @return 返回值
     */
    virtual bool paramCancel(const std::string &name, int transId, double data) = 0;
    /**
     * @brief 表达式计算
     *
     * @param expr 表达式，支持加减乘除与或非等，传入格式为<温湿度2#0> + <温湿度2#1>
              尖括号内为设备名称与点号，以井号分隔
     * @return double
     */
    virtual bool getExprValue(DATA_TYPE type, const std::string &expr, base::ValueWrap &data) = 0;

};
}

#endif /* __DATAENGINE_IDATAENGINE_H__ */
