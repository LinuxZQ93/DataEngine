#ifndef __DATAENGINE_SRC_PROTOCOL_PROTOCOLMANAGER_H__
#define __DATAENGINE_SRC_PROTOCOL_PROTOCOLMANAGER_H__

#include <string>
#include <vector>
#include <mutex>
#include "SingTon/SingTon.h"
#include "Thread/Thread.h"
#include "json/json.h"
#include "IDataEngine.h"

namespace engine {

class IProtocol;
class CProtocolManager {
using FuncPtr = void(*)(const char *);
struct ProtocolInfo {
    IProtocol *pProt;
    std::vector<std::string> vecDevName;
};
SINGTON_DECLAR(CProtocolManager)
public:
    bool registerProtocol(const std::string &taskName, IProtocol *pProtocol);
    bool createProtocol(const std::map<std::string, std::vector<Json::Value>> &mapCfgProtocol);
    bool start();

public:
    bool controlSelect(const std::string &devName, int pointId, CTRL_STATUS status);
    bool controlExecute(const std::string &name, int transId, CTRL_STATUS status);
    bool controlCancel(const std::string &name, int transId, CTRL_STATUS status);
    bool paramSelect(const std::string &name, int transId, double data);
    bool paramExecute(const std::string &name, int transId, double data);
    bool paramCancel(const std::string &name, int transId, double data);

private:
    bool loadProtocolLib(const std::string &libName, const std::string &taskName);
    void process(base::ThreadImpl *pThread, std::vector<ProtocolInfo> vecProt);

private:
    std::mutex m_mutex;
    std::map<std::string, std::vector<IProtocol *>> m_mapProtocol;
    std::map<std::string, IProtocol *> m_mapProtocolPtr;
    std::map<std::string, std::vector<Json::Value>> m_mapValue;
    std::vector<base::ThreadWrap<void, base::ThreadImpl*, std::vector<ProtocolInfo>>*> m_vecThread;
};
}

#endif /* __DATAENGINE_SRC_PROTOCOL_PROTOCOLMANAGER_H__ */
