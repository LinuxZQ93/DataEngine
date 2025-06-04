#ifndef __DATAENGINE_DATAENGINE_H__
#define __TINYRUN_TINYRUN_H__

#include "IDataEngine.h"

namespace engine {

class CDataEngine : public IDataEngine {
SIMPLE_DEF_C(DataEngine, "DataEngine")

public:
    virtual bool init();
    virtual bool start();
    virtual bool stop();
    virtual bool destroy();

public:
    virtual bool getDevicecfg(const std::string &name, CfgType type, Json::Value &cfg);
    virtual bool getTranscfg(const std::string &name, CfgType type, Json::Value &cfg);
    virtual bool attachConnectStatusFunc(const std::string &name, const connectFunc &func);
    virtual bool attachMsgAsyncFunc(const std::string &name, const msgFunc &func);
    virtual bool closeLink(const std::string &name, int fd = -1);
    virtual bool sendFrame(const std::string &name, const std::string &sendBuf);
    virtual bool sendFrame(int fd, const std::string &name, const std::string &sendBuf);
    virtual bool recvFrame(const std::string &name, std::string &recvBuf, int timeOut);
    virtual bool sendSyncRecv(const std::string &name, std::mutex *mutex, DATA_TYPE type, const std::string &sendBuf, std::string &recvBuf, int timeOut);
    virtual void printSendFrame(const std::string &name, DATA_TYPE type, const std::string &frameBuf);
    virtual void printSendFrame(const std::string &name, const std::string &strType, const std::string &frameBuf);
    virtual void printRecvFrame(const std::string &name, DATA_TYPE type, const std::string &frameBuf);
    virtual void printRecvFrame(const std::string &name, const std::string &strType, const std::string &frameBuf);
    virtual bool updateValue(const std::string &name, DATA_TYPE type, int pointId, const base::ValueWrap &data);
    virtual bool updateMultiValue(const std::string &name, DATA_TYPE type, int pointId, const std::vector<base::ValueWrap> &vecData);
    virtual bool updateValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const base::ValueWrap &data);
    virtual bool updateMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const std::vector<base::ValueWrap> &vecData);
    virtual bool getValue(const std::string &name, DATA_TYPE type, int pointId, base::ValueWrap &data);
    virtual bool getMultiValue(const std::string &name, DATA_TYPE type, int pointId, int num, std::vector<base::ValueWrap> &vecDataNew);
    virtual bool getValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, base::ValueWrap &data);
    virtual bool getMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, int num, std::vector<base::ValueWrap> &vecDataNew);
    virtual bool getTransValue(const std::string &name, DATA_TYPE type, int transId, base::ValueWrap &data);
    virtual bool getTransMultiValue(const std::string &name, DATA_TYPE type, int transId, int num, std::vector<base::ValueWrap> &data);
    virtual bool controlSelect(const std::string &name, int transId, CTRL_STATUS status);
    virtual bool controlExecute(const std::string &name, int transId, CTRL_STATUS status);
    virtual bool controlCancel(const std::string &name, int transId, CTRL_STATUS status);
    virtual bool paramSelect(const std::string &name, int transId, double data);
    virtual bool paramExecute(const std::string &name, int transId, double data);
    virtual bool paramCancel(const std::string &name, int transId, double data);
    virtual bool getExprValue(DATA_TYPE type, const std::string &expr, base::ValueWrap &data);

private:
    std::map<DATA_TYPE, std::string> m_mapType;
};

}

#endif /* __TINYRUN_TINYRUN_H__ */
