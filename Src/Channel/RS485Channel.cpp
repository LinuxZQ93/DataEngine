#include "RS485Channel.h"

#include "Config/ConfigParse.h"
#include "Config/IConfigManager.h"

#include "Tool/Tool.h"

namespace engine {

CRS485MsgHandle::CRS485MsgHandle()
{}

CRS485MsgHandle::~CRS485MsgHandle()
{}

void CRS485MsgHandle::handleMessage(int clientId, const char *buf, int len, std::string &response)
{
    #if 0
    for (int i = 0; i < 4; i++)
    {
        rawf("%02x ", buf[i]);
    }
    rawf("\n");
    #endif
    CChannelMsgHandle::handleMessage(buf, len);
}

CRS485Channel::CRS485Channel()
{
}

CRS485Channel::~CRS485Channel()
{
    stop();
}

bool CRS485Channel::init(const Json::Value &cfg)
{
    Json::Value cfgChannelParamTemp;
    if (!CConfigParse::instance()->getTemplateParamCfg("RS-485", CHANNEL, "comName", cfgChannelParamTemp))
    {
        return false;
    }
    auto *pConfig = base::CComponentManager::instance()->getComponent<base::IConfigManager>("ConfigManager");
    Json::Value cfgType;
    pConfig->getConfig("serialType", cfgType);
    int nameValue = cfg["comName"].asInt();
    const std::string &nameRange = cfgChannelParamTemp["range"].asString();
    std::vector<std::string> result;
    splitStr("#", nameRange, result);
    const std::string &comName = result[nameValue];
    const std::string &realName = "/dev/" + cfgType["RS-485"][comName].asString();
    base::CSerial::Config cfgSerial;
    cfgSerial.portName = realName;
    cfgSerial.baudrate = (base::CSerial::BAUDRATE)cfg["baudrate"].asInt();
    cfgSerial.databits = (base::CSerial::DATABITS)cfg["databits"].asInt();
    cfgSerial.parity = (base::CSerial::PARITY)cfg["parity"].asInt();
    cfgSerial.stopbits = (base::CSerial::STOPBITS)cfg["stopbits"].asInt();
    m_pMsgHandle.reset(new CRS485MsgHandle());
    m_channelName = "RS-485:" + realName;
    m_pMsgHandle->setChannelName(m_channelName);
    const std::string &taskName = cfg["taskName"].asString();
    m_pMsgHandle->setTaskName(taskName);
    m_pSerial.reset(new base::CSerial(cfgSerial, std::dynamic_pointer_cast<CRS485MsgHandle>(m_pMsgHandle)));
    return true;
}

bool CRS485Channel::start()
{
    bool ret = m_pSerial->start();
    if (ret && m_connectedFunc)
    {
        m_connectedFunc(CONNECT, m_pSerial->getSocketFd());
    }
    return ret;
}

bool CRS485Channel::stop()
{
    bool ret = m_pSerial->stop();
    if (ret && m_connectedFunc)
    {
        m_connectedFunc(DISCONNECT, m_pSerial->getSocketFd());
    }
    return ret;
}

bool CRS485Channel::sendFrame(const char *buf, int len)
{
    return m_pSerial->send(buf, len);
}

bool CRS485Channel::closeLink(int fd)
{
    return m_pSerial->stop();
}

}
