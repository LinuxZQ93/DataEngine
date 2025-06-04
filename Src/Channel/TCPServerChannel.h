#ifndef __DATAENGINE_SRC_CHANNEL_TCPSERVERCHANNEL_H__
#define __DATAENGINE_SRC_CHANNEL_TCPSERVERCHANNEL_H__

#include "IChannel.h"

#include <memory>
#include "Channel/Net/TcpServer.h"

namespace engine {

class CTCPServerMsgHandle : public base::ITcpServerHandleMessage, public CChannelMsgHandle {
public:
    CTCPServerMsgHandle();
    virtual ~CTCPServerMsgHandle();

public:
    bool attachConnectStatusFunc(const connectFunc &func) { m_func = func; return true;}
    bool attachMsgAsyncFunc(const msgFunc &func) {m_msgFunc = func; return true;}
public:
    virtual void clientConnected(const std::string &ip, int port, int clientId);
    virtual void clientDisconnect(const std::string &ip, int port, int clientId);
    virtual void handleMessage(int clientId, const char *buf, int len, std::string &response);

private:
    connectFunc m_func;
    msgFunc m_msgFunc;
};

class CTCPServerChannel : public CChannelBase {
public:
    CTCPServerChannel();
    virtual ~CTCPServerChannel();

    virtual bool init(const Json::Value &cfg);
    virtual bool start();
    virtual bool stop();

public:
    virtual bool attachConnectStatusFunc(const connectFunc &func);
    virtual bool attachMsgAsyncFunc(const msgFunc &func);
    virtual bool sendFrame(const char *buf, int len);
    virtual bool sendFrame(int fd, const char *buf, int len);
    virtual bool closeLink(int fd);

private:
    std::shared_ptr<base::CTcpServer> m_pTcpSrv;
};

}

#endif /* __DATAENGINE_SRC_CHANNEL_TCPSERVERCHANNEL_H__ */
