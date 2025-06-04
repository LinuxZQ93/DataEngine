#include "Log.h"

#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include "Print/Print.h"
#include "Time/Time.h"
#include "Function/Bind.h"
#include "Tool/Tool.h"

namespace engine {

using std::setw;

CLog::CLog()
{}

CLog::~CLog()
{}

bool CLog::init(const std::string &name, const std::string &channelName, const Json::Value &cfgValue)
{
    m_name = name;
    m_maxLogNum = cfgValue["logLimitNum"].asInt();
    m_maxLogSize = cfgValue["logLimitSize"].asInt();
    if (m_maxLogNum < 0 || m_maxLogSize < 0)
    {
        warnf("logNum[%d], logSize[%d] is invalid\n", m_maxLogNum, m_maxLogSize);
        return false;
    }
    processLog();
    if (!createFile())
    {
        return false;
    }
    updateLogNum();
    attachLogFunc(name, base::function(&CLog::saveLog, this));
    attachLogFunc(channelName, base::function(&CLog::saveLog, this));
    m_fLength = 0;
    return true;
}

void CLog::processLog()
{
    const std::string &logPath = LOG_PATH + m_name;
    struct stat st;
    int ret = stat(logPath.c_str(), &st);
    if (ret == -1)
    {
        mkdir(logPath.c_str(), 0777);
    }
    auto f = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        const std::string &strPath = dirName + "/" + subName;
        if (fType == F_REG)
        {
            if (strPath.find(".log") != std::string::npos)
            {
                m_minFName.insert(strPath);
            }
        }
        return true;
    };
    openFilePath(logPath, f);
}

bool CLog::createFile()
{
    std::stringstream stream;
    stream.fill('0');
    base::CTime::SysTime sTime;
    base::CTime::getSysTime(sTime);
    stream << m_name << "_"
           << sTime.year
           << setw(2) << sTime.mon
           << setw(2) << sTime.day << "_"
           << setw(2) << sTime.hour
           << setw(2) << sTime.min
           << setw(2) << sTime.sec
           << ".log";
    std::string curLogName = stream.str();
    const auto &iter = m_minFName.begin();
    if (iter != m_minFName.end())
    {
        const std::string &minFname = *iter;
        if (curLogName < minFname)
        {
            for (const auto &iter1 : m_minFName)
            {
                remove(iter1.c_str());
            }
            m_minFName.clear();
        }
    }
    const std::string &logPath = LOG_PATH + m_name + "/" + curLogName;
    m_minFName.insert(logPath);
    m_fLog = fopen(logPath.c_str(), "a");
    if (m_fLog == nullptr)
    {
        warnf("create log file[%s] failed\n", logPath.c_str());
        return false;
    }
    return true;
}

void CLog::closeFile()
{
    if (m_fLog)
    {
        fclose(m_fLog);
        m_fLog = NULL;
    }
    m_fLength = 0;
}

void CLog::updateLogNum()
{
    while (m_minFName.size() > m_maxLogNum)
    {
        const std::string &fName = *m_minFName.begin();
        remove(fName.c_str());
        m_minFName.erase(m_minFName.begin());
    }
}

void CLog::saveLog(const std::string &buffer)
{
    if (NULL == m_fLog)
    {
        return;
    }
    if ((m_fLength >> 20) > m_maxLogSize)
    {
        updateLogNum();
        closeFile();
        createFile();
    }
    fwrite(buffer.c_str(), 1, buffer.size(), m_fLog);
    fflush(m_fLog);
    m_fLength += buffer.size();
}
}
