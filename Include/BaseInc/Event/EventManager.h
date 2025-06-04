#ifndef __FRAMEWORK_INCLUDE_EVENT_EVENTMANAGER_H__
#define __FRAMEWORK_INCLUDE_EVENT_EVENTMANAGER_H__

#include "IEventManager.h"
#include "Component/ComponentMacroDef.h"

#include <string>
#include <map>
#include <mutex>

#include "SingTon/SingTon.h"
#include "Thread/Thread.h"
#include "json/json.h"

namespace base {

class CEventManager : public IEventManager
{
    SIMPLE_DEF_C(EventManager, "EventManager");
public:
    virtual bool init();
    virtual bool start();
    virtual bool stop();
public:

    bool attach(const std::string &code, const Proc &proc, SigEnum::SlotPosition pos = SigEnum::posBack, const unsigned int specPos = 0);
    bool detach(const std::string &code, const Proc &proc);

    bool sendEvent(const std::string &code, const Json::Value &cfgEvent);
    bool postEvent(const std::string &code, const Json::Value &cfgEvent);

    bool attach(const std::string &code, const ProcAny &proc, SigEnum::SlotPosition pos = SigEnum::posBack, const unsigned int specPos = 0);
    bool detach(const std::string &code, const ProcAny &proc);

    bool sendEvent(const std::string &code, const base::any &value, base::any &result);
    bool attachRpc();
    bool sendRpcEvent(const std::string &code, const Json::Value &cfgEvent);
    /// 目前RPC只支持Json格式
    bool sendRpcEvent(const std::string &code, const base::any &value, base::any &result);

private:
    void postEventInternal(std::string code, Json::Value cfgEvent);
    void sendRpcEvent(const Json::Value &request, Json::Value &response);
    void sendRpcEventAny(const Json::Value &request, Json::Value &response);

private:
    std::map<std::string, Signal> m_mapEvent;
    std::mutex  m_mutex;
    base::ThreadPool<void> m_pool;
    std::mutex m_mutexAny;
    std::map<std::string, SignalAny> m_mapEventAny;
    std::string m_strAddr;
};

}

#endif /* ifndef __FRAMEWORK_INCLUDE_EVENT_EVENTMANAGER_H__ */
