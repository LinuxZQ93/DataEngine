#include "CollModbus.h"

namespace engine {

CCollModbus::CCollModbus()
{}

CCollModbus::~CCollModbus()
{}

bool CCollModbus::init(const Json::Value &cfg)
{
    infof("CCollModbus::init cfg: %s\n", cfg.toStyledString().c_str());
    return true;
}

bool CCollModbus::process(const std::vector<std::string> &vecDeviceName)
{
    #if 0
    static bool bFirst = true;
    if (!bFirst)
    {
        return true;
    }
    for (auto &deviceName : vecDeviceName)
    {
        infof("CCollModbus::process deviceName: %s\n", deviceName.c_str());
        Json::Value cfgPoint;
        m_dataEngine->getDevicecfg(deviceName, POINTTABLE, cfgPoint);
        infof("CCollModbus::process cfgPoint: %s\n", cfgPoint.toStyledString().c_str());
    }
    bFirst = false;
    #endif
    for (auto &deviceName : vecDeviceName)
    {
        std::string sendBuf(8, '\0');
        sendBuf[0] = 0x01;
        sendBuf[1] = 0x03;
        sendBuf[2] = 0x00;
        sendBuf[3] = 0x00;
        sendBuf[4] = 0x00;
        sendBuf[5] = 0x02;
        sendBuf[6] = 0x71;
        sendBuf[7] = 0xCB;
        std::string recvBuf;
        if (!m_pDataEngine->sendSyncRecv(deviceName, m_mutex, MEASURE, sendBuf, recvBuf, 1000))
        {
            warnf("CCollModbus::process[%s] recvFrame failed\n", deviceName.c_str());
            continue;
        }
        infof("CCollModbus::process[%s] recvBuf: \n", deviceName.c_str());
        m_pDataEngine->printRecvFrame(deviceName, MEASURE, recvBuf);
        m_pDataEngine->updateValue(deviceName, MEASURE, 0, "abcd");
        base::ValueWrap result;
        m_pDataEngine->getValue(deviceName, MEASURE, 0, result);
        infof("CCollModbus::process[%s] result: %s\n", deviceName.c_str(), result.asString().c_str());
        std::pair<std::string, base::ValueWrap> condition;
        condition.first = "addr";
        condition.second = 2;
        m_pDataEngine->updateValue(deviceName, MEASURE, condition, 1.5);
        m_pDataEngine->getValue(deviceName, MEASURE, condition, result);
        condition.first = "addr";
        condition.second = 3;
        m_pDataEngine->updateValue(deviceName, MEASURE, condition, 1.76);
        m_pDataEngine->getValue(deviceName, MEASURE, condition, result);
        infof("CCollModbus::process[%s] measure result: %f\n", deviceName.c_str(), result.asDouble());
        m_pDataEngine->updateValue(deviceName, SIGNAL, 0, 1);
        m_pDataEngine->getValue(deviceName, SIGNAL, 0, result);
        infof("CCollModbus::process[%s] signal result: %d\n", deviceName.c_str(), result.asInt8());
        m_pDataEngine->getExprValue(MEASURE, "<温湿度3#0> + <温湿度2#0>", result);
        infof("expr result %f\n", result.asDouble());
    }
    return true;
}

FIX_FRAME_VALUE CCollModbus::fixFrame(const std::string &devName, const char *buf, int len, int &validLen)
{
    infof("CCollModbus::fixFrame[%s] len: %d\n", devName.c_str(), len);
    if (len < 8)
    {
        return INVALID;
    }
    validLen = len;
    return COMPLETE;
}

bool CCollModbus::controlSelect(const std::string &devName, int pointId, CTRL_STATUS status)
{
    std::string buf(8, '\0');
    buf[0] = 0x01;
    buf[1] = 0x05;
    buf[2] = 0x00;
    buf[3] = 0x01;
    buf[4] = 0x02;
    buf[5] = 0x00;
    buf[6] = 0x03;
    buf[7] = 0x04;
    std::string recvBuf;
    m_pDataEngine->sendSyncRecv(devName, m_mutex, CONTROL, buf, recvBuf, 1000);
    return true;
}

bool CCollModbus::controlExecute(const std::string &devName, int pointId, CTRL_STATUS status)
{
    std::string buf(8, '\0');
    buf[0] = 0x01;
    buf[1] = 0x06;
    buf[2] = 0x00;
    buf[3] = 0x01;
    buf[4] = 0x02;
    buf[5] = 0x00;
    buf[6] = 0x03;
    buf[7] = 0x04;
    std::string recvBuf;
    m_pDataEngine->sendSyncRecv(devName, m_mutex, CONTROL, buf, recvBuf, 1000);
    return true;
}

bool CCollModbus::paramExecute(const std::string &devName, int transId, double data)
{
    std::string buf(8, '\0');
    buf[0] = 0x01;
    buf[1] = 0x07;
    buf[2] = 0x00;
    buf[3] = 0x01;
    buf[4] = 0x02;
    buf[5] = 0x00;
    buf[6] = 0x03;
    buf[7] = 0x04;
    infof("recv data is %lf\n", data);
    std::string recvBuf;
    m_pDataEngine->sendSyncRecv(devName, m_mutex, PARAM, buf, recvBuf, 1000);
    return true;
}

std::string CCollModbus::getProtocolName()
{
    return "CollModbus";
}

extern "C"
{
    void initProtocol(const char *name)
    {
        CCollModbus::CCollModbusProtocolFactory s_collModbus(name);
    }
}

}
