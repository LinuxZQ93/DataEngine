#include "UDPChannel.h"

namespace engine {

CUDPMsgHandle::CUDPMsgHandle()
{}

CUDPMsgHandle::~CUDPMsgHandle()
{}

void CUDPMsgHandle::handleMessage(int clientId, const char *buf, int len, std::string &response)
{
    CChannelMsgHandle::handleMessage(buf, len);
}

CUDPChannel::CUDPChannel()
{}

CUDPChannel::~CUDPChannel()
{
    stop();
}

bool CUDPChannel::init(const Json::Value &cfg)
{
    const std::string &localIP = cfg["localIP"].asString();
    int localPort = cfg["localPort"].asInt();
    int remotePort = cfg["remotePort"].asInt();
    const std::string &remoteIP = cfg["remoteIP"].asString();
    const std::string &taskName = cfg["taskName"].asString();
    m_pMsgHandle.reset(new CUDPMsgHandle());
    m_channelName = "UDP#" + remoteIP + ":" + std::to_string(remotePort);
    m_pMsgHandle->setChannelName(m_channelName);
    m_pMsgHandle->setTaskName(taskName);
    m_pUdp.reset(new base::CUdp(localIP, localPort, remoteIP, remotePort, std::dynamic_pointer_cast<CUDPMsgHandle>(m_pMsgHandle)));
    return true;
}

bool CUDPChannel::start()
{
    bool ret = m_pUdp->start();
    if (ret && m_connectedFunc)
    {
        m_connectedFunc(CONNECT, m_pUdp->getSocketFd());
    }
    return ret;
}

bool CUDPChannel::stop()
{
    bool ret = m_pUdp->stop();
    if (ret && m_connectedFunc)
    {
        m_connectedFunc(DISCONNECT, m_pUdp->getSocketFd());
    }
    return ret;
}

bool CUDPChannel::sendFrame(const char *buf, int len)
{
    return m_pUdp->send(buf, len);
}

bool CUDPChannel::closeLink(int fd)
{
    return m_pUdp->stop();
}

}
