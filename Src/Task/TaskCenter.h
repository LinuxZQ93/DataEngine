#ifndef __DATAENGINE_SRC_TASK_TASKCENTER_H__
#define __DATAENGINE_SRC_TASK_TASKCENTER_H__

#include "SingTon/SingTon.h"
#include "json/json.h"
#include "Config/IConfigManager.h"
#include "IDataEngine.h"

namespace engine {
class CTaskCenter {
SINGTON_DECLAR(CTaskCenter)

public:
    bool init();
    bool start();

public:
    bool attachConnectStatusFunc(const std::string &name, const connectFunc &func);
    bool attachMsgAsyncFunc(const std::string &name, const msgFunc &func);
    bool closeLink(const std::string &name, int fd);
    bool sendFrame(const std::string &name, const std::string &sendBuf);
    bool sendFrame(int fd, const std::string &name, const std::string &sendBuf);
    bool recvFrame(const std::string &name, std::string &recvBuf, int timeOut);
    bool updateValue(const std::string &name, DATA_TYPE type, int pointId, const base::ValueWrap &data);
    bool updateMultiValue(const std::string &name, DATA_TYPE type, int pointId, const std::vector<base::ValueWrap> &vecData);
    bool updateValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const base::ValueWrap &data);
    bool updateMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const std::vector<base::ValueWrap> &vecData);
    bool getValue(const std::string &name, DATA_TYPE type, int pointId, base::ValueWrap &data);
    bool getMultiValue(const std::string &name, DATA_TYPE type, int pointId, int num, std::vector<base::ValueWrap> &vecDataNew);
    bool getValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, base::ValueWrap &data);
    bool getMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, int num, std::vector<base::ValueWrap> &vecDataNew);
    bool getTransValue(const std::string &name, DATA_TYPE type, int transId, base::ValueWrap &data);
    bool getTransMultiValue(const std::string &name, DATA_TYPE type, int transId, int num, std::vector<base::ValueWrap> &data);
    bool controlSelect(const std::string &name, int transId, CTRL_STATUS status);
    bool controlExecute(const std::string &name, int transId, CTRL_STATUS status);
    bool controlCancel(const std::string &name, int transId, CTRL_STATUS status);
    bool paramSelect(const std::string &name, int transId, double data);
    bool paramExecute(const std::string &name, int transId, double data);
    bool paramCancel(const std::string &name, int transId, double data);
    bool getExprValue(DATA_TYPE type, const std::string &expr, base::ValueWrap &data);

private:
    bool arrangeCfg();
    bool arrangeCollCfg(const Json::Value &cfgAttr, std::map<std::string, Json::Value> &mapProtocol);
    bool arrangeTransCfg(const Json::Value &cfgAttr, std::map<std::string, Json::Value> &mapProtocol);
    bool bValidTransDot(const std::string &devName, int pointId, const std::string &strType);

private:
    base::IConfigManager *m_pConfig;
    std::vector<Json::Value> m_vecChannelCfg;
    std::map<std::string, std::vector<Json::Value>> m_mapProtocolCfg;
};
}

#endif /* __DATAENGINE_SRC_TASK_TASKCENTER_H__ */
