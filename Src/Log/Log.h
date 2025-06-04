#ifndef __DATAENGINE_SRC_LOG_LOG_H__
#define __DATAENGINE_SRC_LOG_LOG_H__

#include <string>
#include <set>
#include <mutex>
#include "json/json.h"

namespace engine {

#define LOG_PATH std::string("./log/engine/")

class CLog {
public:
    CLog();
    ~CLog();

public:
    bool init(const std::string &name, const std::string &channelName, const Json::Value &cfgValue);

private:
    void processLog();
    bool createFile();
    void closeFile();
    void updateLogNum();
    void saveLog(const std::string &buffer);

private:
    std::string m_name;
    FILE    *m_fLog;
    unsigned int m_fLength;
    unsigned int m_maxLogSize;
    unsigned int m_maxLogNum;
    std::set<std::string> m_minFName;
};

}

#endif /* __DATAENGINE_SRC_LOG_LOG_H__ */
