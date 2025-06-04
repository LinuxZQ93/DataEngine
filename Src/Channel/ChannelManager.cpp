#include "ChannelManager.h"
#include "Config/ConfigParse.h"
#include "RS485Channel.h"
#include "TCPServerChannel.h"
#include "TCPClientChannel.h"
#include "UDPChannel.h"

namespace engine {

SINGTON_DEFINITION(CChannelManager);

CChannelManager::CChannelManager()
{
}

CChannelManager::~CChannelManager()
{
}

bool CChannelManager::createChannel(const std::vector<Json::Value> &vecCfgChannel)
{
    bool ret = true;
    for (auto &cfgChannel : vecCfgChannel)
    {
        if (!cfgChannel.isMember("template") || !cfgChannel.isMember("content"))
        {
            warnf("channel cfg[%s] error\n", cfgChannel.toStyledString().c_str());
            continue;
        }
        const std::string &strType = cfgChannel["template"].asString();
        /// 未使用通道
        if (strType.empty())
        {
            continue;
        }
        std::shared_ptr<IChannel> pChannel;
        if (createChannelPtr(strType, pChannel))
        {
            pChannel->init(cfgChannel["content"]);
            pChannel->attachDealMsg(base::function(&CChannelManager::dealMsg, this));
            for (uint32_t i = 0; i < cfgChannel["names"].size(); i++)
            {
                const std::string &name = cfgChannel["names"][i].asString();
                m_mapChannel[name] = pChannel;
            }
            m_vecChannel.push_back(pChannel);
        }
    }
    return ret;
}

bool CChannelManager::start()
{
    for (const auto &iter : m_vecChannel)
    {
        iter->start();
    }
    return true;
}

void CChannelManager::attachFixFunc(const std::string &devName, const fixFunc &func)
{
    infof("attachFixFunc:%s\n", devName.c_str());
    std::lock_guard<std::mutex> Guard(m_mutexFunc);
    m_mapFixFunc[devName] = func;
}

std::string CChannelManager::getChannelName(const std::string &devName)
{
    return m_mapChannel[devName]->getChannelName();
}

bool CChannelManager::attachConnectStatusFunc(const std::string &name, const connectFunc &func)
{
    if (m_mapChannel.find(name) == m_mapChannel.end())
    {
        errorf("unknow channel:%s\n", name.c_str());
        return false;
    }
    return m_mapChannel[name]->attachConnectStatusFunc(func);
}

bool CChannelManager::attachMsgAsyncFunc(const std::string &name, const msgFunc &func)
{
    if (m_mapChannel.find(name) == m_mapChannel.end())
    {
        errorf("unknow channel:%s\n", name.c_str());
        return false;
    }
    return m_mapChannel[name]->attachMsgAsyncFunc(func);
}

bool CChannelManager::closeLink(const std::string &name, int fd)
{
    if (m_mapChannel.find(name) == m_mapChannel.end())
    {
        errorf("unknow channel:%s\n", name.c_str());
        return false;
    }
    return m_mapChannel[name]->closeLink(fd);
}

bool CChannelManager::sendFrame(const std::string &name, const char *buf, int len)
{
    if (m_mapChannel.find(name) == m_mapChannel.end())
    {
        errorf("unknow channel:%s\n", name.c_str());
        return false;
    }
    return m_mapChannel[name]->sendFrame(buf, len);
}

bool CChannelManager::recvFrame(const std::string &name, std::string &recvBuf, int timeOut)
{
    if (m_mapChannel.find(name) == m_mapChannel.end())
    {
        errorf("unknow channel:%s\n", name.c_str());
        return false;
    }
    return m_mapChannel[name]->recvFrame(name, recvBuf, timeOut);
}

bool CChannelManager::sendFrame(int fd, const std::string &name, const char *buf, int len)
{
    if (m_mapChannel.find(name) == m_mapChannel.end())
    {
        errorf("unknow channel:%s\n", name.c_str());
        return false;
    }
    return m_mapChannel[name]->sendFrame(fd, buf, len);
}

bool CChannelManager::createChannelPtr(const std::string &strType, std::shared_ptr<IChannel> &ptr)
{
    if (strType == "RS-485")
    {
        ptr = std::make_shared<CRS485Channel>();
    }
    else if (strType == "RS-232")
    {
    }
    else if (strType == "TCPServer")
    {
        ptr = std::make_shared<CTCPServerChannel>();
    }
    else if (strType == "TCPClient")
    {
        ptr = std::make_shared<CTCPClientChannel>();
    }
    else if (strType == "UDP")
    {
        ptr = std::make_shared<CUDPChannel>();
    }
    else
    {
        errorf("unknow channel type:%s\n", strType.c_str());
        return false;
    }
    return true;
}

void CChannelManager::dealMsg(const std::string &devName, std::string &recvBuf)
{
    if (m_mapFixFunc.find(devName) == m_mapFixFunc.end())
    {
        errorf("unknow fix func:%s\n", devName.c_str());
        return;
    }
    int validLen = 0;
    std::string validFrame;
    FIX_FRAME_VALUE ret = m_mapFixFunc[devName](devName, recvBuf.c_str(), (int)recvBuf.length(), validLen);
    if (ret == FIX_FRAME_VALUE::INCOMPLETE)
    {
        return;
    }
    else if (ret == FIX_FRAME_VALUE::COMPLETE)
    {
        validFrame = recvBuf.substr(0, validLen);
        recvBuf.erase(0, validLen);
    }
    else
    {
        recvBuf.clear();
    }
    if (ret == FIX_FRAME_VALUE::COMPLETE)
    {
        m_mapChannel[devName]->recvComplete(devName, validFrame);
    }
}

}
