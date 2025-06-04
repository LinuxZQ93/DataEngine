#ifndef __DATAENGINE_SRC_CHANNEL_ICHANNEL_H__
#define __DATAENGINE_SRC_CHANNEL_ICHANNEL_H__

#include <string>
#include <mutex>
#include "Function/Bind.h"
#include "Semaphore/Semaphore.h"
#include "Thread/Thread.h"
#include "json/json.h"
#include "IDataEngine.h"

namespace engine {

using dealMsgfunc = TFunction<void, const std::string&, std::string&>;

class CChannelMsgHandle {
public:
    CChannelMsgHandle();
    virtual ~CChannelMsgHandle();

public:
    void setName(const std::string &name);
    void attachDealMsg(const dealMsgfunc &func);

    void setChannelName(const std::string &channelName){m_channelName = channelName;}
    void setTaskName(const std::string &taskName){m_taskName = taskName;}

public:
    void handleMessage(const char *buf, int len);

private:
    void dealMessage();

protected:
    dealMsgfunc m_funcDealMsg;
    std::mutex m_mutex;
    std::string m_name;
    std::string m_channelName;
    std::string m_taskName;
    std::string m_recvBuf;
    base::ThreadWrap<void> m_thread;
};


class IChannel {
public:
    virtual ~IChannel() {}

    virtual bool init(const Json::Value &cfg) = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;

public:
    virtual void attachDealMsg(const dealMsgfunc &func) = 0;
    virtual bool attachConnectStatusFunc(const connectFunc &func) = 0;
    virtual bool attachMsgAsyncFunc(const msgFunc &func) = 0;
    virtual bool closeLink(int fd) = 0;
    virtual bool sendFrame(const char *buf, int len) = 0;
    virtual bool sendFrame(int fd, const char *buf, int len) = 0;
    virtual bool recvFrame(const std::string &name, std::string &recvBuf, int timeOut) = 0;
    virtual void recvComplete(const std::string &name, const std::string &recvBuf) = 0;
    virtual std::string getChannelName() = 0;

};

class CChannelBase : public IChannel {
public:
    CChannelBase(){}
    virtual ~CChannelBase(){}

public:
    virtual void attachDealMsg(const dealMsgfunc &func);
    virtual bool attachConnectStatusFunc(const connectFunc &func) { m_connectedFunc = func; return true; }
    virtual bool attachMsgAsyncFunc(const msgFunc &func) {return false;}
    virtual bool sendFrame(int fd, const char *buf, int len){ return true; };
    virtual bool recvFrame(const std::string &name, std::string &recvBuf, int timeOut);
    virtual void recvComplete(const std::string &name, const std::string &recvBuf);
    virtual bool cleanRecvBuffer(const std::string &name);
    virtual std::string getChannelName() { return m_channelName; }

protected:
    std::map<std::string, Semaphore*> m_mapSem;
    std::string m_recvBuf;
    std::shared_ptr<CChannelMsgHandle> m_pMsgHandle;
    std::string m_channelName;
    connectFunc m_connectedFunc;
};

}

#endif /* __DATAENGINE_SRC_CHANNEL_ICHANNEL_H__ */