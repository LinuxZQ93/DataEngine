#include "TaskCenter.h"
#include "Tool/Tool.h"

#include "Config/ConfigParse.h"
#include "Channel/ChannelManager.h"
#include "Protocol/ProtocolManager.h"
#include "Data/DataStorage.h"
#include "Log/LogManager.h"

namespace engine {

SINGTON_DEFINITION(CTaskCenter)

CTaskCenter::CTaskCenter()
{}

CTaskCenter::~CTaskCenter()
{}

bool CTaskCenter::init()
{
    m_pConfig = base::CComponentManager::instance()->getComponent<base::IConfigManager>("ConfigManager");
    if (!CDataStorage::instance()->init())
    {
        return false;
    }
    if (!arrangeCfg())
    {
        return false;
    }
    CChannelManager::instance()->createChannel(m_vecChannelCfg);
    CProtocolManager::instance()->createProtocol(m_mapProtocolCfg);
    std::vector<std::string> vecName;
    CConfigParse::instance()->getDeviceName(vecName);
    for (auto &devName : vecName)
    {
        Json::Value cfgAttr;
        CConfigParse::instance()->getDeviceCfg(devName, ATTRIBUTE, cfgAttr);
        const std::string &channelName = CChannelManager::instance()->getChannelName(devName);
        CLogManager::instance()->init(devName, channelName, cfgAttr);
    }
    vecName.clear();
    CConfigParse::instance()->getTransName(vecName);
    for (auto &transName : vecName)
    {
        Json::Value cfgAttr;
        CConfigParse::instance()->getTransCfg(transName, ATTRIBUTE, cfgAttr);
        const std::string &channelName = CChannelManager::instance()->getChannelName(transName);
        CLogManager::instance()->init(transName, channelName, cfgAttr);
    }
    return true;
}

bool CTaskCenter::start()
{
    CChannelManager::instance()->start();
    CProtocolManager::instance()->start();
    return true;
}

bool CTaskCenter::attachConnectStatusFunc(const std::string &name, const connectFunc &func)
{
    return CChannelManager::instance()->attachConnectStatusFunc(name, func);
}

bool CTaskCenter::attachMsgAsyncFunc(const std::string &name, const msgFunc &func)
{
    return CChannelManager::instance()->attachMsgAsyncFunc(name, func);
}

bool CTaskCenter::closeLink(const std::string &name, int fd)
{
    return CChannelManager::instance()->closeLink(name, fd);
}

bool CTaskCenter::sendFrame(const std::string &name, const std::string &sendBuf)
{
    return CChannelManager::instance()->sendFrame(name, sendBuf.c_str(), sendBuf.size());
}

bool CTaskCenter::sendFrame(int fd, const std::string &name, const std::string &sendBuf)
{
    return CChannelManager::instance()->sendFrame(fd, name, sendBuf.c_str(), sendBuf.size());
}

bool CTaskCenter::recvFrame(const std::string &name, std::string &recvBuf, int timeOut)
{
    return CChannelManager::instance()->recvFrame(name, recvBuf, timeOut);
}

bool CTaskCenter::updateValue(const std::string &name, DATA_TYPE type, int pointId, const base::ValueWrap &data)
{
    return CDataStorage::instance()->updateValue(name, type, pointId, data);
}

bool CTaskCenter::updateMultiValue(const std::string &name, DATA_TYPE type, int pointId, const std::vector<base::ValueWrap> &vecData)
{
    return CDataStorage::instance()->updateMultiValue(name, type, pointId, vecData);
}

bool CTaskCenter::updateValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const base::ValueWrap &data)
{
    return CDataStorage::instance()->updateValue(name, type, condition, data);
}

bool CTaskCenter::updateMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const std::vector<base::ValueWrap> &vecData)
{
    return CDataStorage::instance()->updateMultiValue(name, type, condition, vecData);
}

bool CTaskCenter::getValue(const std::string &name, DATA_TYPE type, int pointId, base::ValueWrap &data)
{
    return CDataStorage::instance()->getValue(name, type, pointId, data);
}

bool CTaskCenter::getMultiValue(const std::string &name, DATA_TYPE type, int pointId, int num, std::vector<base::ValueWrap> &vecData)
{
    return CDataStorage::instance()->getMultiValue(name, type, pointId, num, vecData);
}

bool CTaskCenter::getValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, base::ValueWrap &data)
{
    return CDataStorage::instance()->getValue(name, type, condition, data);
}

bool CTaskCenter::getMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, int num, std::vector<base::ValueWrap> &vecData)
{
    return CDataStorage::instance()->getMultiValue(name, type, condition, num, vecData);
}

bool CTaskCenter::getTransValue(const std::string &name, DATA_TYPE type, int transId, base::ValueWrap &data)
{
    return CDataStorage::instance()->getTransValue(name, type, transId, data);
}

bool CTaskCenter::getTransMultiValue(const std::string &name, DATA_TYPE type, int transId, int num, std::vector<base::ValueWrap> &vecData)
{
    return CDataStorage::instance()->getTransMultiValue(name, type, transId, num, vecData);
}

bool CTaskCenter::controlSelect(const std::string &name, int transId, CTRL_STATUS status)
{
    std::string devName;
    int pointId = -1;
    std::string strType("control");
    if (!CConfigParse::instance()->transInfoToDevInfo(name, strType, transId, devName, pointId))
    {
        return false;
    }
    if (!bValidTransDot(devName, pointId, strType))
    {
        errorf("trans[%s], dot[%d] not find in[%s] control cfg, please recreate transTable\n", name.c_str(), transId, devName.c_str());
        return false;
    }
    return CProtocolManager::instance()->controlSelect(devName, pointId, status);
}

bool CTaskCenter::controlExecute(const std::string &name, int transId, CTRL_STATUS status)
{
    std::string devName;
    int pointId = -1;
    std::string strType("control");
    if (!CConfigParse::instance()->transInfoToDevInfo(name, strType, transId, devName, pointId))
    {
        return false;
    }
    if (!bValidTransDot(devName, pointId, strType))
    {
        errorf("trans[%s], dot[%d] not find in[%s] control cfg, please recreate transTable\n", name.c_str(), transId, devName.c_str());
        return false;
    }
    return CProtocolManager::instance()->controlExecute(devName, pointId, status);
}

bool CTaskCenter::controlCancel(const std::string &name, int transId, CTRL_STATUS status)
{
    std::string devName;
    int pointId = -1;
    std::string strType("control");
    if (!CConfigParse::instance()->transInfoToDevInfo(name, strType, transId, devName, pointId))
    {
        return false;
    }
    if (!bValidTransDot(devName, pointId, strType))
    {
        errorf("trans[%s], dot[%d] not find in[%s] control cfg, please recreate transTable\n", name.c_str(), transId, devName.c_str());
        return false;
    }
    return CProtocolManager::instance()->controlCancel(devName, pointId, status);
}

bool CTaskCenter::paramSelect(const std::string &name, int transId, double data)
{
    std::string devName;
    int pointId = -1;
    std::string strType("param");
    if (!CConfigParse::instance()->transInfoToDevInfo(name, strType, transId, devName, pointId))
    {
        return false;
    }
    if (!bValidTransDot(devName, pointId, strType))
    {
        errorf("trans[%s], dot[%d] not find in[%s] param cfg, please recreate transTable\n", name.c_str(), transId, devName.c_str());
        return false;
    }
    return CProtocolManager::instance()->paramSelect(devName, pointId, data);
}

bool CTaskCenter::paramExecute(const std::string &name, int transId, double data)
{
    std::string devName;
    int pointId = -1;
    std::string strType("param");
    if (!CConfigParse::instance()->transInfoToDevInfo(name, strType, transId, devName, pointId))
    {
        return false;
    }
    if (!bValidTransDot(devName, pointId, strType))
    {
        errorf("trans[%s], dot[%d] not find in[%s] param cfg, please recreate transTable\n", name.c_str(), transId, devName.c_str());
        return false;
    }
    return CProtocolManager::instance()->paramExecute(devName, pointId, data);
}

bool CTaskCenter::paramCancel(const std::string &name, int transId, double data)
{
    std::string devName;
    int pointId = -1;
    std::string strType("param");
    if (!CConfigParse::instance()->transInfoToDevInfo(name, strType, transId, devName, pointId))
    {
        return false;
    }
    if (!bValidTransDot(devName, pointId, strType))
    {
        errorf("trans[%s], dot[%d] not find in[%s] param cfg, please recreate transTable\n", name.c_str(), transId, devName.c_str());
        return false;
    }
    return CProtocolManager::instance()->paramCancel(devName, pointId, data);
}

bool CTaskCenter::getExprValue(DATA_TYPE type, const std::string &expr, base::ValueWrap &data)
{
    return CDataStorage::instance()->getExprValue(type, expr, data);
}

bool CTaskCenter::arrangeCfg()
{
    std::vector<std::string> vecName;
    CConfigParse::instance()->getTaskName(vecName);
    for (const auto &name : vecName)
    {
        std::map<std::string, Json::Value> mapProtocol;
        Json::Value cfgAttr;
        Json::Value cfgChannel;
        CConfigParse::instance()->getTaskCfg(name, ATTRIBUTE, cfgAttr);
        CConfigParse::instance()->getTaskCfg(name, CHANNEL, cfgChannel);
        Json::Value cfgChannelNew;
        cfgChannelNew["content"] = cfgChannel["content"];
        cfgChannelNew["template"] = cfgChannel["template"];
        for (uint32_t i = 0; i < cfgAttr["content"].size(); i++)
        {
            cfgChannelNew["names"].append(cfgAttr["content"][i]["name"]);
        }
        cfgChannelNew["content"]["taskName"] = name;
        m_vecChannelCfg.push_back(std::move(cfgChannelNew));

        bool bTrans = cfgAttr["bTrans"].asBool();
        if (!bTrans)
        {
            arrangeCollCfg(cfgAttr, mapProtocol);
        }
        else
        {
            arrangeTransCfg(cfgAttr, mapProtocol);
        }

        for (const auto &iter : mapProtocol)
        {
            m_mapProtocolCfg[name].push_back(iter.second);
        }
    }
    return true;
}

bool CTaskCenter::arrangeCollCfg(const Json::Value &cfgAttr, std::map<std::string, Json::Value> &mapProtocol)
{
    for (uint32_t i = 0; i < cfgAttr["content"].size(); i++)
    {
        Json::Value cfgProtocol;
        const std::string &deviceName = cfgAttr["content"][i]["name"].asString();
        const std::string &deviceTypeName = cfgAttr["content"][i]["devTypeName"].asString();
        CConfigParse::instance()->getDeviceCfg(deviceName, PROTOCOL, cfgProtocol);
        if (cfgProtocol.empty())
        {
            warnf("device %s not found protocol cfg!\n", deviceName.c_str());
            continue;
        }
        if (mapProtocol.find(deviceTypeName) == mapProtocol.end())
        {
            cfgProtocol["names"].append(deviceName);
            mapProtocol[deviceTypeName] = cfgProtocol;
        }
        else
        {
            mapProtocol[deviceTypeName]["names"].append(deviceName);
        }
        const std::string &proTempName = cfgProtocol["template"].asString();
        Json::Value cfgTempProt;
        CConfigParse::instance()->getTemplateCfg(proTempName, PROTOCOL, cfgTempProt);
        mapProtocol[deviceTypeName]["plugName"] = cfgTempProt["plugName"];
    }
    return true;
}

bool CTaskCenter::arrangeTransCfg(const Json::Value &cfgAttr, std::map<std::string, Json::Value> &mapProtocol)
{
    /// 转发数组只有一个
    for (uint32_t i = 0; i < cfgAttr["content"].size(); i++)
    {
        Json::Value cfgProtocol;
        const std::string &transName = cfgAttr["content"][i]["name"].asString();
        CConfigParse::instance()->getTransCfg(transName, PROTOCOL, cfgProtocol);
        if (cfgProtocol.empty())
        {
            warnf("device %s not found protocol cfg!\n", transName.c_str());
            continue;
        }
        cfgProtocol["names"].append(transName);
        mapProtocol[transName] = cfgProtocol;
        const std::string &proTempName = cfgProtocol["template"].asString();
        Json::Value cfgTempProt;
        CConfigParse::instance()->getTemplateCfg(proTempName, PROTOCOL, cfgTempProt);
        mapProtocol[transName]["plugName"] = cfgTempProt["plugName"];
    }
    return true;
}

bool CTaskCenter::bValidTransDot(const std::string &devName, int pointId, const std::string &strType)
{
    Json::Value cfgPoint;
    if (!CConfigParse::instance()->getDeviceCfg(devName, POINTTABLE, cfgPoint))
    {
        return false;
    }
    const Json::Value &cfgType = cfgPoint["param"]["control"][pointId];
    return cfgType.empty() ? false : true;
}

}
