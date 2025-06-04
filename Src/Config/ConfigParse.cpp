#include "ConfigParse.h"
#include <dirent.h>
#include <sys/stat.h>

namespace engine {

SINGTON_DEFINITION(CConfigParse);

CConfigParse::CConfigParse()
{
    m_pConfig = base::CComponentManager::instance()->getComponent<base::IConfigManager>("ConfigManager");
}

CConfigParse::~CConfigParse()
{}

bool CConfigParse::parse()
{
    if (!parseTask())
    {
        return false;
    }
    if (!parseDevice())
    {
        return false;
    }
    if (!parseTrans())
    {
        return false;
    }
    if (!parseComposite())
    {
        return false;
    }
    if (!parseTemplate())
    {
        return false;
    }
    return true;
}

bool CConfigParse::getTaskName(std::vector<std::string> &vecName)
{
    for (auto &it : m_mapTaskCfg)
    {
        vecName.push_back(it.first);
    }
    return true;
}

bool CConfigParse::getDeviceName(std::vector<std::string> &vecName)
{
    for (auto &it : m_mapDeviceNameCfg)
    {
        vecName.push_back(it.first);
    }
    return true;
}

bool CConfigParse::getTransName(std::vector<std::string> &vecName)
{
    for (auto &it : m_mapTransNameCfg)
    {
        vecName.push_back(it.first);
    }
    return true;
}

bool CConfigParse::getDeviceCfg(const std::string &name, CfgType type, Json::Value &cfg)
{
    if (m_mapDeviceNameCfg.find(name) == m_mapDeviceNameCfg.end())
    {
        return false;
    }
    if (type == ATTRIBUTE)
    {
        cfg = m_mapCfgAttr[name];
        return true;
    }
    if (m_mapDeviceNameCfg[name]->find(type) == m_mapDeviceNameCfg[name]->end())
    {
        return false;
    }
    cfg = (*m_mapDeviceNameCfg[name])[type];
    return true;
}

bool CConfigParse::getTransCfg(const std::string &name, CfgType type, Json::Value &cfg)
{
    if (m_mapTransNameCfg.find(name) == m_mapTransNameCfg.end())
    {
        return false;
    }
    if (type == ATTRIBUTE)
    {
        cfg = m_mapCfgAttr[name];
        return true;
    }
    if (m_mapTransNameCfg[name].find(type) == m_mapTransNameCfg[name].end())
    {
        return false;
    }
    cfg = m_mapTransNameCfg[name][type];
    return true;
}

bool CConfigParse::getTaskCfg(const std::string &name, CfgType type, Json::Value &cfg)
{
    if (m_mapTaskCfg.find(name) == m_mapTaskCfg.end())
    {
        return false;
    }
    if (m_mapTaskCfg[name].find(type) == m_mapTaskCfg[name].end())
    {
        return false;
    }
    cfg = m_mapTaskCfg[name][type];
    return true;
}

bool CConfigParse::parseTask()
{
    const std::string &strTaskPath = ROOT_PATH + TASK_CFG;
    auto f = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        if (fType != F_REG && subName.find(".json") == std::string::npos)
        {
            return true;
        }
        const std::string &strPath = dirName + "/" + subName;
        Json::Value cfgTask;
        if (!m_pConfig->parseFileConfig(strPath, cfgTask))
        {
            errorf("parse task cfg[%s] failed\n", strPath.c_str());
            return true;
        }
        bool bEnable = cfgTask["enable"].asBool();
        if (!bEnable)
        {
            return true;
        }
        const std::string &name = cfgTask["name"].asString();
        const Json::Value &cfgTaskAttr = cfgTask["param"]["attribute"];
        m_mapTaskCfg[name][ATTRIBUTE] = cfgTaskAttr;
        m_mapTaskCfg[name][CHANNEL] = cfgTask["param"]["channel"];
        return true;
    };
    openFilePath(strTaskPath, f);
    return true;
}

bool CConfigParse::parseDevice()
{
    const std::string &strDevicePath = ROOT_PATH + DEVSTORE_CFG;
    std::string devName;
    auto f1 = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        const std::string &strPath = dirName + "/" + subName;
        if (fType != F_REG && subName.find(".json") == std::string::npos)
        {
            return true;
        }
        Json::Value cfgValue;
        if (!m_pConfig->parseFileConfig(strPath, cfgValue))
        {
            errorf("parse device cfg[%s] failed\n", strPath.c_str());
            return true;
        }
        if (subName == "PointTable.json")
        {
            m_mapDeviceCfg[devName][POINTTABLE] = cfgValue;
        }
        else if (subName == "Protocol.json")
        {
            m_mapDeviceCfg[devName][PROTOCOL] = cfgValue;
        }
        return true;
    };
    auto f = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        const std::string &strPath = dirName + "/" + subName;
        if (fType == F_DIR)
        {
            devName = subName;
            return openFilePath(strPath, f1);
        }
        return true;
    };
    openFilePath(strDevicePath, f);
    for (auto &iter : m_mapTaskCfg)
    {
        bool bTrans = iter.second[ATTRIBUTE]["bTrans"].asBool();
        for (auto &iter1 : iter.second[ATTRIBUTE]["content"])
        {
            const std::string &devName = iter1["name"].asString();
            if (bTrans)
            {
                /// 转发任务只有一个content
                m_mapCfgAttr[devName] = iter1;
                continue;
            }
            const std::string &devTypeName = iter1["devTypeName"].asString();
            if (m_mapDeviceCfg.find(devTypeName) == m_mapDeviceCfg.end())
            {
                errorf("device[%s] not found\n", devTypeName.c_str());
                continue;
            }
            m_mapDeviceNameCfg[devName] = &m_mapDeviceCfg[devTypeName];
            m_mapCfgAttr[devName] = iter1;
        }
    }
    return true;
}

bool CConfigParse::parseTrans()
{
    const std::string &strDevicePath = ROOT_PATH + TRANS_CFG;
    std::string transName;
    auto f1 = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        const std::string &strPath = dirName + "/" + subName;
        if (fType != F_REG && subName.find(".json") == std::string::npos)
        {
            return true;
        }
        Json::Value cfgValue;
        if (!m_pConfig->parseFileConfig(strPath, cfgValue))
        {
            errorf("parse device cfg[%s] failed\n", strPath.c_str());
            return true;
        }
        if (subName == "PointTable.json")
        {
            m_mapTransNameCfg[transName][POINTTABLE] = cfgValue;
        }
        else if (subName == "Protocol.json")
        {
            m_mapTransNameCfg[transName][PROTOCOL] = cfgValue;
        }
        return true;
    };
    auto f = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        const std::string &strPath = dirName + "/" + subName;
        if (fType == F_DIR)
        {
            transName = subName;
            return openFilePath(strPath, f1);
        }
        return true;
    };
    openFilePath(strDevicePath, f);
    return true;
}

bool CConfigParse::parseComposite()
{
    const std::string &strPath = ROOT_PATH + "Composite.json";
    Json::Value cfgValue;
    if (!m_pConfig->parseFileConfig(strPath, cfgValue))
    {
        errorf("parse composite cfg[%s] failed\n", strPath.c_str());
        return true;
    }
    m_compositeCfg = cfgValue;
    return true;
}

bool CConfigParse::getTemplateCfg(const std::string &name, CfgType type, Json::Value &cfg)
{
    if (m_mapTemplateCfg.find(name) == m_mapTemplateCfg.end())
    {
        return false;
    }
    auto &cfgMap = m_mapTemplateCfg[name];
    if (cfgMap.find(type) == cfgMap.end())
    {
        return false;
    }
    cfg = cfgMap[type];
    return true;
}

bool CConfigParse::getTemplateParamCfg(const std::string &tempName, CfgType type, const std::string &paramName, Json::Value &cfg)
{
    if (type == POINTTABLE)
    {
        warnf("this functon not support pointTable cfg\n");
        return false;
    }
    Json::Value cfgTemp;
    if (!getTemplateCfg(tempName, type, cfgTemp))
    {
        return false;
    }
    const Json::Value &cfgParam = cfgTemp["param"];
    for (uint32_t i = 0; i < cfgParam.size(); ++i)
    {
        if (cfgParam[i]["name"].asString() == paramName)
        {
            cfg = cfgParam[i];
            return true;
        }
    }
    return false;
}

bool CConfigParse::getTemplateParamCfg(const std::string &tempName, const std::string &dataType, const std::string &paramName, Json::Value &cfg)
{
    static std::set<std::string> setDataType{"signal", "measure", "pulse", "control", "param"};
    if (setDataType.find(dataType) == setDataType.end())
    {
        warnf("data type[%s] not support\n", dataType.c_str());
        return false;
    }
    Json::Value cfgTemp;
    if (!getTemplateCfg(tempName, POINTTABLE, cfgTemp))
    {
        return false;
    }
    const Json::Value &cfgParam = cfgTemp["param"][dataType];
    for (uint32_t i = 0; i < cfgParam.size(); ++i)
    {
        if (cfgParam[i]["name"].asString() == paramName)
        {
            cfg = cfgParam[i];
            return true;
        }
    }
    return false;
}

bool CConfigParse::parseTemplate()
{
    const std::string &attrTemp = ROOT_PATH + TEMPLATE_CFG + "Attribute/";
    const std::string &channelTemp = ROOT_PATH + TEMPLATE_CFG + "Channel/";
    const std::string &pointTemp = ROOT_PATH + TEMPLATE_CFG + "PointTable/";
    const std::string &protocolTemp = ROOT_PATH + TEMPLATE_CFG + "Protocol/";
    auto f = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        if (fType != F_REG && subName.find(".json") == std::string::npos)
        {
            return true;
        }
        const std::string &strPath = dirName + "/" + subName;
        Json::Value cfgAttr;
        if (!m_pConfig->parseFileConfig(strPath, cfgAttr))
        {
            errorf("parse template attribute cfg[%s] failed\n", strPath.c_str());
            return false;
        }
        const std::string &name = cfgAttr["name"].asString();
        m_mapTemplateCfg[name][ATTRIBUTE] = cfgAttr;
        return true;
    };
    auto f1 = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        if (fType != F_REG && subName.find(".json") == std::string::npos)
        {
            return true;
        }
        const std::string &strPath = dirName + "/" + subName;
        Json::Value cfgChannel;
        if (!m_pConfig->parseFileConfig(strPath, cfgChannel))
        {
            errorf("parse template channel cfg[%s] failed\n", strPath.c_str());
            return false;
        }
        const std::string &name = cfgChannel["name"].asString();
        m_mapTemplateCfg[name][CHANNEL] = cfgChannel;
        return true;
    };
    auto f2 = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        if (fType != F_REG && subName.find(".json") == std::string::npos)
        {
            return true;
        }
        const std::string &strPath = dirName + "/" + subName;
        Json::Value cfgPointTable;
        if (!m_pConfig->parseFileConfig(strPath, cfgPointTable))
        {
            errorf("parse template pointTable cfg[%s] failed\n", strPath.c_str());
            return false;
        }
        const std::string &name = cfgPointTable["name"].asString();
        m_mapTemplateCfg[name][POINTTABLE] = cfgPointTable;
        return true;
    };
    auto f3 = [&](const std::string &dirName, const std::string &subName, FILE_TYPE fType)
    {
        if (fType != F_REG && subName.find(".json") == std::string::npos)
        {
            return true;
        }
        const std::string &strPath = dirName + "/" + subName;
        Json::Value cfgProtocol;
        if (!m_pConfig->parseFileConfig(strPath, cfgProtocol))
        {
            errorf("parse template protocol cfg[%s] failed\n", strPath.c_str());
            return false;
        }
        const std::string &name = cfgProtocol["name"].asString();
        m_mapTemplateCfg[name][PROTOCOL] = cfgProtocol;
        return true;
    };
    openFilePath(attrTemp, f);
    openFilePath(channelTemp, f1);
    openFilePath(pointTemp, f2);
    openFilePath(protocolTemp, f3);
    return true;
}

bool CConfigParse::bPrintSource(const std::string &name)
{
    const Json::Value &cfgChannel = m_mapTaskCfg[name][CHANNEL];
    return cfgChannel["enablePrintSource"].asBool();
}

bool CConfigParse::bPrintFrame(const std::string &name)
{
    const Json::Value &cfgAttr = m_mapCfgAttr[name];
    return cfgAttr["extra"]["enablePrintFrame"].asBool();
}

bool CConfigParse::transInfoToDevInfo(const std::string &transName, const std::string &strType, int transId, std::string &devName, int &pointId)
{
    Json::Value cfgTransPoint;
    if (!getTransCfg(transName, POINTTABLE, cfgTransPoint))
    {
        return false;
    }
    const Json::Value &cfgTransDot = cfgTransPoint["param"][strType][transId];
    if (cfgTransDot.empty())
    {
        errorf("trans[%s] type[%s] transId[%d] not found\n", transName.c_str(), strType.c_str(), transId);
        return false;
    }
    devName = cfgTransDot["devName"].asString();
    pointId = cfgTransDot["pointId"].asInt();
    return true;
}

}
