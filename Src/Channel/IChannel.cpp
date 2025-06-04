#include "IChannel.h"

#include "Print/Print.h"
#include "Tool/Tool.h"
#include "Config/ConfigParse.h"

namespace engine {

CChannelMsgHandle::CChannelMsgHandle()
    : m_thread("processBuf")
{
    m_thread.setParam(base::function(&CChannelMsgHandle::dealMessage, this));
    m_thread();
}

CChannelMsgHandle::~CChannelMsgHandle()
{}

void CChannelMsgHandle::setName(const std::string &name)
{
    std::lock_guard<std::mutex> Guard(m_mutex);
    m_name = name;
}

void CChannelMsgHandle::attachDealMsg(const dealMsgfunc &func)
{
    m_funcDealMsg = func;
}

void CChannelMsgHandle::printSource(const std::string &recvBuf)
{
    bool bPrint = CConfigParse::instance()->bPrintSource(m_taskName);
    printBuf(m_channelName, SOURCE, "未知", RECV, recvBuf, bPrint);
}

void CChannelMsgHandle::handleMessage(const char *buf, int len)
{
    std::string recvBuf(buf, len);
    printSource(recvBuf);
    m_mutex.lock();
    m_recvBuf += recvBuf;
    m_mutex.unlock();
}

void CChannelMsgHandle::dealMessage()
{
    std::string recvBufTmp;
    std::string name;
    while (m_thread.looping())
    {
        m_mutex.lock();
        name = m_name;
        m_mutex.unlock();
        if (!name.empty())
        {
            m_mutex.lock();
            recvBufTmp += m_recvBuf;
            m_recvBuf.clear();
            m_mutex.unlock();
            if (!recvBufTmp.empty())
            {
                m_funcDealMsg(name, recvBufTmp);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void CChannelBase::attachDealMsg(const dealMsgfunc &func)
{
    m_pMsgHandle->attachDealMsg(func);
}

bool CChannelBase::recvFrame(const std::string &name, std::string &recvBuf, int timeOut)
{
    if (m_mapSem.find(name) == m_mapSem.end())
    {
        m_mapSem[name] = new Semaphore();
    }

    while (m_mapSem[name]->tryPend())
    {
    }

    m_pMsgHandle->setName(name);
    if (!m_mapSem[name]->pendFor(timeOut))
    {
        m_pMsgHandle->setName("");
        return false;
    }
    recvBuf = m_recvBuf;
    return true;
}

void CChannelBase::recvComplete(const std::string &name, const std::string &recvBuf)
{
    m_recvBuf = recvBuf;
    m_mapSem[name]->post();
    m_pMsgHandle->setName("");
}

bool CChannelBase::cleanRecvBuffer(const std::string &name)
{
    while(m_mapSem[name]->tryPend())
    {
    }
    m_recvBuf.clear();
    return true;
}

}
