#ifndef __FRAMEWORK_INCLUDE_LOG_LOG_H__
#define __FRAMEWORK_INCLUDE_LOG_LOG_H__

#include "Log/LogBase.h"

#include <set>
#include <vector>

#include "json/json.h"

#include "SingTon/SingTon.h"
#include "Export/Export.h"

namespace base {

class DLL_EXPORT CLog : public CLogBase {
SINGTON_DECLAR(CLog)
public:
    virtual bool init();
    bool destroy();

public:
    virtual void saveLog(const char *buf, int len);

private:
    void processLog(bool bDel);
    bool createFile();
    void setHead();
    void closeFile();
    void updateLogFileNum();

private:
    std::mutex m_mutex;
    FILE    *m_fLog;
    unsigned int m_fLength;
    unsigned int m_maxLogSize;
    unsigned int m_maxLogNum;
    std::string m_curLogName;
    std::set<std::string> m_minFName;
};

}

#endif /* __FRAMEWORK_INCLUDE_LOG_LOG_H__ */
