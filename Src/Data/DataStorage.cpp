#include "DataStorage.h"

#include "Tool/ExpParser.h"

namespace engine {

SINGTON_DEFINITION(CDataStorage)

CDataStorage::CDataStorage()
{}

CDataStorage::~CDataStorage()
{}

bool CDataStorage::init()
{
    std::map<DATA_TYPE, std::string> mapType{{SIGNAL, "signal"}, {MEASURE, "measure"},
                                             {PULSE, "pulse"}, {CONTROL, "control"}, {PARAM, "param"}};
    m_mapType.swap(mapType);
    std::vector<std::string> vecName;
    CConfigParse::instance()->getTaskName(vecName);
    for (const auto &iter : vecName)
    {
        Json::Value cfgAttr;
        CConfigParse::instance()->getTaskCfg(iter, ATTRIBUTE, cfgAttr);
        bool bTrans = cfgAttr["bTrans"].asBool();
        if (bTrans)
        {
            continue;
        }
        for (const auto &iter1 : cfgAttr["content"])
        {
            const std::string &devName = iter1["name"].asString();
            Json::Value cfgPoint;
            CConfigParse::instance()->getDeviceCfg(devName, POINTTABLE, cfgPoint);
            initData(devName, cfgPoint);
        }
    }
    return true;
}

bool CDataStorage::updateValue(const std::string &name, DATA_TYPE type, int pointId, const base::ValueWrap &data)
{
    std::lock_guard<std::mutex> Guard(m_mutex);
    if (m_mapValue.find(name) == m_mapValue.end())
    {
        return false;
    }
    auto &vecData = m_mapValue[name][type];
    if (pointId < 0 || pointId >= (int)vecData.size())
    {
        return false;
    }
    vecData[pointId] = data;
    return true;
}

bool CDataStorage::updateMultiValue(const std::string &name, DATA_TYPE type, int pointId, const std::vector<base::ValueWrap> &vecDataNew)
{
    std::lock_guard<std::mutex> Guard(m_mutex);
    if (m_mapValue.find(name) == m_mapValue.end())
    {
        return false;
    }
    auto &vecData = m_mapValue[name][type];
    if (pointId < 0 || pointId >= (int)vecData.size())
    {
        return false;
    }
    int minSize = std::min((int)vecData.size(), (int)vecDataNew.size() + pointId);
    for (int i = 0; i < minSize; ++i)
    {
        vecData[pointId + i] = vecDataNew[i];
    }
    return true;
}

bool CDataStorage::updateValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const base::ValueWrap &data)
{
    int index = getIndex(name, type, condition);
    if (index < 0)
    {
        return false;
    }
    return updateValue(name, type, index, data);
}

bool CDataStorage::updateMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const std::vector<base::ValueWrap> &vecDataNew)
{
    int index = getIndex(name, type, condition);
    if (index < 0)
    {
        return false;
    }
    return updateMultiValue(name, type, index, vecDataNew);
}

bool CDataStorage::getValue(const std::string &name, DATA_TYPE type, int pointId, base::ValueWrap &data)
{
    std::lock_guard<std::mutex> Guard(m_mutex);
    if (m_mapValue.find(name) == m_mapValue.end())
    {
        return false;
    }
    auto &vecData = m_mapValue[name][type];
    if (pointId < 0 || pointId >= (int)vecData.size())
    {
        return false;
    }
    data = vecData[pointId];
    return true;
}

bool CDataStorage::getMultiValue(const std::string &name, DATA_TYPE type, int pointId, int num, std::vector<base::ValueWrap> &vecDataNew)
{
    std::lock_guard<std::mutex> Guard(m_mutex);
    if (m_mapValue.find(name) == m_mapValue.end())
    {
        return false;
    }
    auto &vecData = m_mapValue[name][type];
    if (pointId < 0 || pointId >= (int)vecData.size())
    {
        return false;
    }
    int minSize = std::min((int)vecData.size(), num + pointId);
    for (int i = 0; i < minSize; ++i)
    {
        vecDataNew.push_back(vecData[pointId + i]);
    }
    return true;
}

bool CDataStorage::getValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, base::ValueWrap &data)
{
    int index = getIndex(name, type, condition);
    if (index < 0)
    {
        return false;
    }
    return getValue(name, type, index, data);
}

bool CDataStorage::getMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, int num, std::vector<base::ValueWrap> &vecDataNew)
{
    int index = getIndex(name, type, condition);
    if (index < 0)
    {
        return false;
    }
    return getMultiValue(name, type, index, num, vecDataNew);
}

bool CDataStorage::getTransValue(const std::string &name, DATA_TYPE type, int transId, base::ValueWrap &data)
{
    if (m_mapType.find(type) == m_mapType.end())
    {
        return false;
    }
    std::string devName;
    int pointId = -1;
    if (!CConfigParse::instance()->transInfoToDevInfo(name, m_mapType[type], transId, devName, pointId))
    {
        return false;
    }
    if (devName == "Composite")
    {
        return getCompositeValue(type, pointId, data);
    }
    return getValue(devName, type, pointId, data);
}

bool CDataStorage::getTransMultiValue(const std::string &name, DATA_TYPE type, int transId, int num, std::vector<base::ValueWrap> &vecDataNew)
{
    if (m_mapType.find(type) == m_mapType.end())
    {
        return false;
    }
    for (int i = 0; i < num; i++)
    {
        std::string devName;
        int pointId = -1;
        transId += i;
        if (!CConfigParse::instance()->transInfoToDevInfo(name, m_mapType[type], transId, devName, pointId))
        {
            return false;
        }
        base::ValueWrap data;
        if (devName == "Composite")
        {
            if (!getCompositeValue(type, pointId, data))
            {
                return false;
            }
        }
        else
        {
            if (!getValue(devName, type, pointId, data))
            {
                return false;
            }
        }
        vecDataNew.push_back(data);
    }
    return true;
}

bool CDataStorage::getExprValue(DATA_TYPE type, const std::string &expr, base::ValueWrap &data)
{
    auto f = [&](const std::string &str) {
        std::vector<std::string> result;
        splitStr("#", str, result);
        int pointId = std::stoi(result[1]);
        std::string dst;
        base::ValueWrap value;
        if (!getValue(result[0], type, pointId, value))
        {
            warnf("composite value error\n");
            return dst;
        }
        if (!transValueToString(type, value, dst))
        {
            return dst;
        }
        return dst;
    };
    std::string formula = expr;
    replaceRangeStr("<", ">", f, formula);
    CExpParser tmp(formula);

    if (type == MEASURE)
    {
        data = tmp.calculate();
    }
    else if (type == SIGNAL)
    {
        data = uint8_t((int64_t)tmp.calculate() & 0xff);
    }
    else if (type == PULSE)
    {

    }
    return true;
}

void CDataStorage::initData(const std::string &devName, const Json::Value &cfgPoint)
{
    const Json::Value &cfgParam = cfgPoint["param"];
    std::map<std::string, DATA_TYPE> mapType{{"signal", SIGNAL}, {"measure", MEASURE},
                                             {"pulse", PULSE}, {"control", CONTROL}, {"param", PARAM}};
    const auto &member = cfgParam.getMemberNames();
    for (const auto &iter : member)
    {
        const Json::Value &cfgData = cfgParam[iter];
        int num = (int)cfgData.size();
        std::vector<base::ValueWrap> vecValue(num, 0);
        m_mapValue[devName][mapType[iter]].swap(vecValue);
    }
}

int CDataStorage::getIndex(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition)
{
    int index = -1;
    if (m_mapType.find(type) == m_mapType.end())
    {
        return index;
    }
    m_mutex.lock();
    if (m_mapValue.find(name) == m_mapValue.end())
    {
        m_mutex.unlock();
        return index;
    }
    m_mutex.unlock();
    const std::string &strType = m_mapType[type];
    Json::Value cfgPoint;
    CConfigParse::instance()->getDeviceCfg(name, POINTTABLE, cfgPoint);
    const std::string &tempName = cfgPoint["template"].asString();
    Json::Value cfgPointTemp;
    CConfigParse::instance()->getTemplateParamCfg(tempName, strType, condition.first, cfgPointTemp);
    int dataType = cfgPointTemp["dataType"].asInt();
    Json::Value cfgCond;
    if (dataType == 0)
    {
        cfgCond[condition.first] = condition.second.asInt();
    }
    else if (dataType == 1)
    {
        cfgCond[condition.first] = condition.second.asDouble();
    }
    else if (dataType == 2)
    {
        cfgCond[condition.first] = condition.second.asString();
    }
    else
    {
        warnf("data type error\n");
        return index;
    }
    const Json::Value &cfgData = cfgPoint["param"][strType];
    for (uint32_t i = 0; i < cfgData.size(); ++i)
    {
        if (cfgData[i][condition.first] == cfgCond[condition.first])
        {
            index = i;
            break;
        }
    }
    return index;
}

double CDataStorage::transValueToDouble(const base::ValueWrap &value)
{
    auto type = value.getType();
    double result;
    switch (type)
    {
    case base::ValueWrap::ValueType::INT32:
        result = value.asInt();
        break;
    case base::ValueWrap::ValueType::DOUBLE:
        result = value.asDouble();
        break;
    case base::ValueWrap::ValueType::INT8:
        result = value.asInt8();
        break;
    case base::ValueWrap::ValueType::FLOAT:
        result = value.asFloat();
        break;
    case base::ValueWrap::ValueType::UINT8:
        result = value.asUInt8();
        break;
    case base::ValueWrap::ValueType::UINT32:
        result = value.asUInt32();
        break;
    case base::ValueWrap::ValueType::UINT64:
        result = value.asUInt64();
        break;
    case base::ValueWrap::ValueType::INT64:
        result = value.asInt64();
        break;
    default:
        result = 0;
        break;
    }
    return result;
}

bool CDataStorage::transValueToString(DATA_TYPE type, const base::ValueWrap &value, std::string &str)
{
    if (type == MEASURE)
    {
        if (value.isString() || value.empty())
        {
            warnf("composite value type error\n");
            return false;
        }
        double result = transValueToDouble(value);
        str = std::to_string(result);
    }
    else if (type == SIGNAL)
    {
        if (!value.isInt8() && !value.isUInt8() && !value.isInt() && !value.isBool())
        {
            warnf("composite value type[%d] error\n", value.getType());
            return false;
        }
        uint8_t result = value.asUInt8();
        str = std::to_string(result);
    }
    else if (type == PULSE)
    {

    }
    return true;
}

bool CDataStorage::getCompositeValue(DATA_TYPE type, int pointId, base::ValueWrap &data)
{
    const std::string &strType = m_mapType[type];
    Json::Value cfgPoint;
    CConfigParse::instance()->getCompositeCfg(cfgPoint);
    const Json::Value &cfgData = cfgPoint["param"][strType];
    if (pointId < 0 || pointId >= (int)cfgData.size())
    {
        return false;
    }
    std::string formula = cfgData[pointId]["formula"].asString();
    auto f = [&](const std::string &str) {
        std::vector<std::string> result;
        splitStr("#", str, result);
        int pointId = std::stoi(result[1]);
        std::string dst;
        base::ValueWrap value;
        if (!getValue(result[0], type, pointId, value))
        {
            warnf("composite value error\n");
            return dst;
        }
        if (!transValueToString(type, value, dst))
        {
            return dst;
        }
        return dst;
    };
    replaceRangeStr("<", ">", f, formula);
    CExpParser tmp(formula);
    if (type == MEASURE)
    {
        data = tmp.calculate();
    }
    else if (type == SIGNAL)
    {
        data = uint8_t((int64_t)tmp.calculate() & 0xff);
    }
    else if (type == PULSE)
    {

    }
    return true;
}

}
