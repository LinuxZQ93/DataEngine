#include "TCPClientChannel.h"

namespace engine {

CTCPClientMsgHandle::CTCPClientMsgHandle()
{}

CTCPClientMsgHandle::~CTCPClientMsgHandle()
{}

void CTCPClientMsgHandle::handleMessage(int clientId, const char *buf, int len, std::string &response)
{
    CChannelMsgHandle::handleMessage(buf, len);
}

CTCPClientChannel::CTCPClientChannel()
{}

CTCPClientChannel::~CTCPClientChannel()
{
    stop();
}

bool CTCPClientChannel::init(const Json::Value &cfg)
{
    int remotePort = cfg["remotePort"].asInt();
    const std::string &remoteIP = cfg["remoteIP"].asString();
    const std::string &taskName = cfg["taskName"].asString();
    m_pMsgHandle.reset(new CTCPClientMsgHandle());
    m_channelName = "TCPCli#" + remoteIP + ":" + std::to_string(remotePort);
    m_pMsgHandle->setChannelName(m_channelName);
    m_pMsgHandle->setTaskName(taskName);
    m_pTcpCli.reset(new base::CTcpClient(remoteIP, remotePort, std::dynamic_pointer_cast<CTCPClientMsgHandle>(m_pMsgHandle)));
    return true;
}

bool CTCPClientChannel::start()
{
    bool ret = m_pTcpCli->start();
    if (ret && m_connectedFunc)
    {
        m_connectedFunc(CONNECT, m_pTcpCli->getSocketFd());
    }
    return ret;
}

bool CTCPClientChannel::stop()
{
    bool ret = m_pTcpCli->stop();
    if (ret && m_connectedFunc)
    {
        m_connectedFunc(DISCONNECT, m_pTcpCli->getSocketFd());
    }
    return ret;
}

bool CTCPClientChannel::sendFrame(const char *buf, int len)
{
    return m_pTcpCli->send(buf, len);
}

bool CTCPClientChannel::closeLink(int fd)
{
    return m_pTcpCli->stop();
}

}
