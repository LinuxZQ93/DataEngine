#ifndef __DATAENGINE_LOG_LOGMANAGER_H__
#define __DATAENGINE_LOG_LOGMANAGER_H__

#include <string>
#include <map>
#include "SingTon/SingTon.h"
#include "json/json.h"

#include "Log.h"

namespace engine {

class CLogManager {
SINGTON_DECLAR(CLogManager)

public:
    bool init(const std::string &name, const std::string &channelName, const Json::Value &cfgValue);

private:
    std::map<std::string, CLog*> m_mapLog;
};

}

#endif /* __DATAENGINE_LOG_LOGMANAGER_H__ */
