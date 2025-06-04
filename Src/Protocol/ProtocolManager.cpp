#include "ProtocolManager.h"

#include "IProtocol.h"
#include "Print/Print.h"
#include "Library/Library.h"
#include "Function/Bind.h"
#include "Channel/ChannelManager.h"

namespace engine {
SINGTON_DEFINITION(CProtocolManager)

CProtocolManager::CProtocolManager()
{}

CProtocolManager::~CProtocolManager()
{}

bool CProtocolManager::registerProtocol(const std::string &taskName, IProtocol *pProtocol)
{
    std::lock_guard<std::mutex> Guard(m_mutex);
    m_mapProtocol[taskName].push_back(pProtocol);
    return true;
}

bool CProtocolManager::createProtocol(const std::map<std::string, std::vector<Json::Value>> &mapCfgProtocol)
{
    for (const auto &iter : mapCfgProtocol)
    {
        for (const auto &iter1 : iter.second)
        {
            const std::string &plugName = iter1["plugName"].asString();
            loadProtocolLib(plugName, iter.first);
            m_mapValue[iter.first + plugName].push_back(iter1);
        }
    }
    return true;
}

bool CProtocolManager::start()
{
    for (const auto &iter : m_mapProtocol)
    {
        std::vector<ProtocolInfo> vecProtInfo;
        for (uint32_t i = 0; i < iter.second.size(); i++)
        {
            const Json::Value &cfgValue = m_mapValue[iter.first][i];
            iter.second[i]->init(cfgValue);
            ProtocolInfo info;
            info.pProt = iter.second[i];
            for (uint32_t j = 0; j < cfgValue["names"].size(); j++)
            {
                const std::string &devName = cfgValue["names"][j].asString();
                info.vecDevName.push_back(devName);
                m_mapProtocolPtr[devName] = info.pProt;
            }
            vecProtInfo.push_back(std::move(info));
        }
        auto *threadWrap = new base::ThreadWrap<void, base::ThreadImpl*, std::vector<ProtocolInfo>>(iter.first, 600);
        threadWrap->setParam(base::bind(base::function(&CProtocolManager::process, this), (base::ThreadImpl *)threadWrap, vecProtInfo));
        m_vecThread.push_back(threadWrap);
    }
    for (const auto &iter : m_vecThread)
    {
        (*iter)();
    }
    return true;
}

bool CProtocolManager::controlSelect(const std::string &devName, int pointId, CTRL_STATUS status)
{
    if (m_mapProtocolPtr.find(devName) == m_mapProtocolPtr.end())
    {
        errorf("control select devName %s not found\n", devName.c_str());
        return false;
    }
    if (!m_mapProtocolPtr[devName]->controlSelect(devName, pointId, status))
    {
        warnf("control select devName %s pointId %d status %d failed\n", devName.c_str(), pointId, (int)status);
        return false;
    }
    infof("control select devName %s pointId %d status %d success\n", devName.c_str(), pointId, (int)status);
    return true;
}

bool CProtocolManager::controlExecute(const std::string &devName, int pointId, CTRL_STATUS status)
{
    if (m_mapProtocolPtr.find(devName) == m_mapProtocolPtr.end())
    {
        errorf("control execute devName %s not found\n", devName.c_str());
        return false;
    }
    if (!m_mapProtocolPtr[devName]->controlExecute(devName, pointId, status))
    {
        warnf("control execute devName %s pointId %d status %d failed\n", devName.c_str(), pointId, (int)status);
        return false;
    }
    infof("control execute devName %s pointId %d status %d success\n", devName.c_str(), pointId, (int)status);
    return true;
}

bool CProtocolManager::controlCancel(const std::string &devName, int pointId, CTRL_STATUS status)
{
    if (m_mapProtocolPtr.find(devName) == m_mapProtocolPtr.end())
    {
        errorf("control cancel devName %s not found\n", devName.c_str());
        return false;
    }
    if (!m_mapProtocolPtr[devName]->controlCancel(devName, pointId, status))
    {
        warnf("control cancel devName %s pointId %d status %d failed\n", devName.c_str(), pointId, (int)status);
        return false;
    }
    infof("control cancel devName %s pointId %d status %d success\n", devName.c_str(), pointId, (int)status);
    return true;
}

bool CProtocolManager::paramSelect(const std::string &devName, int pointId, double data)
{
    if (m_mapProtocolPtr.find(devName) == m_mapProtocolPtr.end())
    {
        errorf("param select devName %s not found\n", devName.c_str());
        return false;
    }
    if (!m_mapProtocolPtr[devName]->paramSelect(devName, pointId, data))
    {
        warnf("param select devName %s pointId %d value %lf failed\n", devName.c_str(), pointId, data);
        return false;
    }
    infof("param select devName %s pointId %d value %lf success\n", devName.c_str(), pointId, data);
    return true;
}

bool CProtocolManager::paramExecute(const std::string &devName, int pointId, double data)
{
    if (m_mapProtocolPtr.find(devName) == m_mapProtocolPtr.end())
    {
        errorf("param execute devName %s not found\n", devName.c_str());
        return false;
    }
    if (!m_mapProtocolPtr[devName]->paramExecute(devName, pointId, data))
    {
        warnf("param execute devName %s pointId %d value %lf failed\n", devName.c_str(), pointId, data);
        return false;
    }
    infof("param execute devName %s pointId %d value %lf success\n", devName.c_str(), pointId, data);
    return true;
}

bool CProtocolManager::paramCancel(const std::string &devName, int pointId, double data)
{
    if (m_mapProtocolPtr.find(devName) == m_mapProtocolPtr.end())
    {
        errorf("param cancel devName %s not found\n", devName.c_str());
        return false;
    }
    if (!m_mapProtocolPtr[devName]->paramCancel(devName, pointId, data))
    {
        warnf("param cancel devName %s pointId %d value %lf failed\n", devName.c_str(), pointId, data);
        return false;
    }
    infof("param cancel devName %s pointId %d value %lf success\n", devName.c_str(), pointId, data);
    return true;
}

bool CProtocolManager::loadProtocolLib(const std::string &plugName, const std::string &taskName)
{
    std::string strErr;
    base::LibraryPtr ptr = base::loadLibraryPtr(base::getLibName(plugName), strErr);
    if (NULL == ptr)
    {
        errorf("%s lib load error, %s\n", plugName.c_str(), strErr.c_str());
        return false;
    }
    FuncPtr fPtr = (FuncPtr)(base::getFuncPtrFormLib(ptr, "initProtocol"));
    if (NULL == fPtr)
    {
        errorf("%s lib get func ptr errors\n", plugName.c_str());
        base::unloadLibraryPtr(ptr);
        return false;
    }
    fPtr((taskName + plugName).c_str());
    return true;
}

void CProtocolManager::process(base::ThreadImpl *pThread, std::vector<ProtocolInfo> vecProt)
{
    std::mutex mutex;
    for (auto &iter : vecProt)
    {
        iter.pProt->setLockSource(&mutex);
        for (auto &iter1 : iter.vecDevName)
        {
            CChannelManager::instance()->attachFixFunc(iter1, base::function(&IProtocol::fixFrame, iter.pProt));
        }
    }
    while (pThread->looping())
    {
        for (auto &iter : vecProt)
        {
            iter.pProt->process(iter.vecDevName);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

}
