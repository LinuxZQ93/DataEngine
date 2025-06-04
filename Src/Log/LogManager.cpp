#include "LogManager.h"

namespace engine {

SINGTON_DEFINITION(CLogManager)

CLogManager::CLogManager()
{}

CLogManager::~CLogManager()
{}

bool CLogManager::init(const std::string &name, const std::string &channelName, const Json::Value &cfgValue)
{
    const Json::Value &cfgExtra = cfgValue["extra"];
    bool bEnableLog = cfgExtra["enableLog"].asBool();
    if (!bEnableLog)
    {
        return true;
    }
    if (m_mapLog.find(name) == m_mapLog.end())
    {
        m_mapLog[name] = new CLog();
    }
    return m_mapLog[name]->init(name, channelName, cfgValue["extra"]);
}

}
