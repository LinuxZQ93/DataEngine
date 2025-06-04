#include "TCPServerChannel.h"

#include "Config/ConfigParse.h"
#include "Config/IConfigManager.h"

#include "Tool/Tool.h"

namespace engine {

CTCPServerMsgHandle::CTCPServerMsgHandle()
{}

CTCPServerMsgHandle::~CTCPServerMsgHandle()
{}

void CTCPServerMsgHandle::clientConnected(const std::string &ip, int port, int clientId)
{
    if (m_func)
    {
        m_func(CONNECT, clientId);
    }
}

void CTCPServerMsgHandle::clientDisconnect(const std::string &ip, int port, int clientId)
{
    if (m_func)
    {
        m_func(DISCONNECT, clientId);
    }
}

void CTCPServerMsgHandle::handleMessage(int clientId, const char *buf, int len, std::string &response)
{
    #if 0
    for (int i = 0; i < 4; i++)
    {
        rawf("%02x ", buf[i]);
    }
    rawf("\n");
    #endif
    if (m_msgFunc)
    {
        CChannelMsgHandle::printSource(std::string(buf, len));
        m_msgFunc(clientId, buf, len);
    }
    else
    {
        CChannelMsgHandle::handleMessage(buf, len);
    }
}

CTCPServerChannel::CTCPServerChannel()
{
}

CTCPServerChannel::~CTCPServerChannel()
{
    stop();
}

bool CTCPServerChannel::init(const Json::Value &cfg)
{
    const std::string &localIP = cfg["localIP"].asString();
    int localPort = cfg["localPort"].asInt();
    const std::string &remoteIP = cfg["remoteIP"].asString();
    const std::string &taskName = cfg["taskName"].asString();
    m_pMsgHandle.reset(new CTCPServerMsgHandle());
    m_channelName = "TCPSrv#" + localIP + ":" + std::to_string(localPort);
    m_pMsgHandle->setChannelName(m_channelName);
    m_pMsgHandle->setTaskName(taskName);
    m_pTcpSrv.reset(new base::CTcpServer(localIP, localPort, std::dynamic_pointer_cast<CTCPServerMsgHandle>(m_pMsgHandle)));
    if (!remoteIP.empty())
    {
        std::vector<std::string> result;
        splitStr("#", remoteIP, result);
        std::set<std::string> setIP;
        if (!result.empty())
        {
            for (auto &ip : result)
            {
                setIP.insert(ip);
            }
        }
        else
        {
            setIP.insert(remoteIP);
        }
        m_pTcpSrv->setWhiteList(setIP);
    }
    return true;
}

bool CTCPServerChannel::start()
{
    return m_pTcpSrv->start();
}

bool CTCPServerChannel::stop()
{
    return m_pTcpSrv->stop();
}

bool CTCPServerChannel::attachConnectStatusFunc(const connectFunc &func)
{
    auto pMsgHandle = std::dynamic_pointer_cast<CTCPServerMsgHandle>(m_pMsgHandle);
    return pMsgHandle->attachConnectStatusFunc(func);
}

bool CTCPServerChannel::attachMsgAsyncFunc(const msgFunc &func)
{
    auto pMsgHandle = std::dynamic_pointer_cast<CTCPServerMsgHandle>(m_pMsgHandle);
    return pMsgHandle->attachMsgAsyncFunc(func);
}

bool CTCPServerChannel::sendFrame(const char *buf, int len)
{
    return m_pTcpSrv->notifyAll(buf, len);
}

bool CTCPServerChannel::sendFrame(int fd, const char *buf, int len)
{
    return m_pTcpSrv->notify(fd, buf, len);
}

bool CTCPServerChannel::closeLink(int fd)
{
    return m_pTcpSrv->closeClient(fd);
}

}