#include "DataEngine.h"

#include "Config/ConfigParse.h"
#include "Task/TaskCenter.h"
#include "Tool/Tool.h"
#include "Tool/ExpParser.h"

namespace engine {

SINGTON_DEFINITION(CDataEngine)

CDataEngine::CDataEngine()
{}

CDataEngine::~CDataEngine()
{}

bool CDataEngine::init()
{
    std::map<DATA_TYPE, std::string> mapType{{SIGNAL, "遥信"}, {MEASURE, "遥测"}, {PULSE, "遥脉"}, {CONTROL, "遥控"}, {PARAM, "遥调"}, {UNKNOWN, "未知"}};
    m_mapType.swap(mapType);
    CConfigParse::instance()->parse();
    std::vector<std::string> vecName;
    CConfigParse::instance()->getTaskName(vecName);
    for (auto& name : vecName)
    {
        infof("task name: %s\n", name.c_str());
    }
    CTaskCenter::instance()->init();
    return true;
}

bool CDataEngine::start()
{
    CTaskCenter::instance()->start();
    return true;
}

bool CDataEngine::stop()
{
    return true;
}

bool CDataEngine::destroy()
{
    return true;
}

bool CDataEngine::getDevicecfg(const std::string &name, CfgType type, Json::Value &cfg)
{
    return CConfigParse::instance()->getDeviceCfg(name, type, cfg);
}

bool CDataEngine::getTranscfg(const std::string &name, CfgType type, Json::Value &cfg)
{
    return CConfigParse::instance()->getTransCfg(name, type, cfg);
}

bool CDataEngine::attachConnectStatusFunc(const std::string &name, const connectFunc &func)
{
    return CTaskCenter::instance()->attachConnectStatusFunc(name, func);
}

bool CDataEngine::attachMsgAsyncFunc(const std::string &name, const msgFunc &func)
{
    return CTaskCenter::instance()->attachMsgAsyncFunc(name, func);
}

bool CDataEngine::closeLink(const std::string &name, int fd)
{
    return CTaskCenter::instance()->closeLink(name, fd);
}

bool CDataEngine::sendFrame(const std::string &name, const std::string &sendBuf)
{
    return CTaskCenter::instance()->sendFrame(name, sendBuf);
}

bool CDataEngine::sendFrame(int fd, const std::string &name, const std::string &sendBuf)
{
    return CTaskCenter::instance()->sendFrame(fd, name, sendBuf);
}

bool CDataEngine::recvFrame(const std::string &name, std::string &recvBuf, int timeOut)
{
    return CTaskCenter::instance()->recvFrame(name, recvBuf, timeOut);
}

bool CDataEngine::sendSyncRecv(const std::string &name, std::mutex *mutex, DATA_TYPE type, const std::string &sendBuf, std::string &recvBuf, int timeOut)
{
    std::lock_guard<std::mutex> Guard(*mutex);
    if (!sendFrame(name, sendBuf))
    {
        return false;
    }
    printSendFrame(name, type, sendBuf);
    return recvFrame(name, recvBuf, timeOut);
}

void CDataEngine::printSendFrame(const std::string &name, DATA_TYPE type, const std::string &frameBuf)
{
    bool bPrint = CConfigParse::instance()->bPrintFrame(name);
    printBuf(name, FRAME, m_mapType[type], SEND, frameBuf, bPrint);
}

void CDataEngine::printSendFrame(const std::string &name, const std::string &strType, const std::string &frameBuf)
{
    bool bPrint = CConfigParse::instance()->bPrintFrame(name);
    printBuf(name, FRAME, strType, SEND, frameBuf, bPrint);
}

void CDataEngine::printRecvFrame(const std::string &name, DATA_TYPE type, const std::string &frameBuf)
{
    bool bPrint = CConfigParse::instance()->bPrintFrame(name);
    printBuf(name, FRAME, m_mapType[type], RECV, frameBuf, bPrint);
}

void CDataEngine::printRecvFrame(const std::string &name, const std::string &strType, const std::string &frameBuf)
{
    bool bPrint = CConfigParse::instance()->bPrintFrame(name);
    printBuf(name, FRAME, strType, RECV, frameBuf, bPrint);
}

bool CDataEngine::updateValue(const std::string &name, DATA_TYPE type, int pointId, const base::ValueWrap &data)
{
    return CTaskCenter::instance()->updateValue(name, type, pointId, data);
}

bool CDataEngine::updateMultiValue(const std::string &name, DATA_TYPE type, int pointId, const std::vector<base::ValueWrap> &vecData)
{
    return CTaskCenter::instance()->updateMultiValue(name, type, pointId, vecData);
}

bool CDataEngine::updateValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const base::ValueWrap &data)
{
    return CTaskCenter::instance()->updateValue(name, type, condition, data);
}

bool CDataEngine::updateMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const std::vector<base::ValueWrap> &vecData)
{
    return CTaskCenter::instance()->updateMultiValue(name, type, condition, vecData);
}

bool CDataEngine::getValue(const std::string &name, DATA_TYPE type, int pointId, base::ValueWrap &data)
{
    return CTaskCenter::instance()->getValue(name, type, pointId, data);
}

bool CDataEngine::getMultiValue(const std::string &name, DATA_TYPE type, int pointId, int num, std::vector<base::ValueWrap> &vecData)
{
    return CTaskCenter::instance()->getMultiValue(name, type, pointId, num, vecData);
}

bool CDataEngine::getValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, base::ValueWrap &data)
{
    return CTaskCenter::instance()->getValue(name, type, condition, data);
}

bool CDataEngine::getMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, int num, std::vector<base::ValueWrap> &vecData)
{
    return CTaskCenter::instance()->getMultiValue(name, type, condition, num, vecData);
}

bool CDataEngine::getTransValue(const std::string &name, DATA_TYPE type, int transId, base::ValueWrap &data)
{
    return CTaskCenter::instance()->getTransValue(name, type, transId, data);
}

bool CDataEngine::getTransMultiValue(const std::string &name, DATA_TYPE type, int transId, int num, std::vector<base::ValueWrap> &vecData)
{
    return CTaskCenter::instance()->getTransMultiValue(name, type, transId, num, vecData);
}

bool CDataEngine::controlSelect(const std::string &name, int transId, CTRL_STATUS status)
{
    return CTaskCenter::instance()->controlSelect(name, transId, status);
}

bool CDataEngine::controlExecute(const std::string &name, int transId, CTRL_STATUS status)
{
    return CTaskCenter::instance()->controlExecute(name, transId, status);
}

bool CDataEngine::controlCancel(const std::string &name, int transId, CTRL_STATUS status)
{
    return CTaskCenter::instance()->controlSelect(name, transId, status);
}

bool CDataEngine::paramSelect(const std::string &name, int transId, double data)
{
    return CTaskCenter::instance()->paramSelect(name, transId, data);
}

bool CDataEngine::paramExecute(const std::string &name, int transId, double data)
{
    return CTaskCenter::instance()->paramExecute(name, transId, data);
}

bool CDataEngine::paramCancel(const std::string &name, int transId, double data)
{
    return CTaskCenter::instance()->paramCancel(name, transId, data);
}

bool CDataEngine::getExprValue(DATA_TYPE type, const std::string &expr, base::ValueWrap &data)
{
    return CTaskCenter::instance()->getExprValue(type, expr, data);
}

extern "C"
{
    void initComponent()
    {
        static CDataEngine::CDataEngineComponentFactory s_data;
    }
}

}
