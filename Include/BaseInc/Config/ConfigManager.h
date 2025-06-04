#ifndef __FRAMEWORK_INCLUDE_CONFIG_CONFIGMANAGER_H__
#define __FRAMEWORK_INCLUDE_CONFIG_CONFIGMANAGER_H__

#include "IConfigManager.h"

#include <string>
#include <map>
#include <mutex>
#include <list>

#include "json/json.h"

#include "SingTon/SingTon.h"
#include "Function/Signal.h"

namespace base {

class CConfigManager : public IConfigManager {
    SIMPLE_DEF_C(ConfigManager, "ConfigManager")
public:
    virtual bool init();
    virtual bool start();
    virtual bool stop();
    virtual bool destroy();

    virtual bool setConfig(const std::string &key, const Json::Value &cfg);
    virtual bool getConfig(const std::string &key, Json::Value &cfg);

    virtual bool setDefault(const std::string &key, const Json::Value &cfg);
    virtual bool getDefault(const std::string &key, Json::Value &cfg);

    virtual bool attachVerifyConfig(const std::string &key, const ProcCheck &func);
    virtual bool detachVerifyConfig(const std::string &key, const ProcCheck &func);
    virtual bool attachChangedConfig(const std::string &key, const ProcChanged &func);
    virtual bool detachChangedConfig(const std::string &key, const ProcChanged &func);

    virtual bool parseFileConfig(const std::string &path, Json::Value &result);

    virtual bool attachChangedConfig(const std::string &key, const ProcChanged &func, SigEnum::SlotPosition pos, const unsigned int specPos = 0);

private:
    void recurseConfig(const std::string &defPath, const std::string &cfgPath);
    void jsonMearge(const Json::Value &dCfg, Json::Value &cCfg);
    void parseJson(const std::string &defPath, const Json::Value &defCfg, const std::string &cfgPath, const Json::Value &cfg);

private:
    std::mutex m_mutexCfg;
    std::map<std::string, Json::Value> m_mapCfg;
    std::mutex m_mutexDefCfg;
    std::map<std::string, Json::Value> m_mapDefCfg;
    std::map<std::string, SignalCheck> m_mapProcCheck;
    std::map<std::string, SignalChanged> m_mapProcChanged;
    std::mutex m_mutexCheck;
    std::mutex m_mutexChanged;
    std::map<std::string, std::string> m_mapPath;
    std::map<std::string, std::string> m_mapDefPath;

    bool m_bCfgLoad;
};

}

#endif /* ifndef __FRAMEWORK_INCLUDE_CONFIG_CONFIGMANAGER_H__ */

