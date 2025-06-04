#ifndef __DATAENGINE_SRC_CHANNEL_TCPCLIENTCHANNEL_H__
#define __DATAENGINE_SRC_CHANNEL_TCPCLIENTCHANNEL_H__

#include "IChannel.h"

#include <memory>
#include "Channel/Net/TcpClient.h"

namespace engine {

class CTCPClientMsgHandle : public base::ITcpClientHandleMessage, public CChannelMsgHandle {
public:
    CTCPClientMsgHandle();
    virtual ~CTCPClientMsgHandle();

public:
    virtual void handleMessage(int clientId, const char *buf, int len, std::string &response);
};

class CTCPClientChannel : public CChannelBase {
public:
    CTCPClientChannel();
    virtual ~CTCPClientChannel();

    virtual bool init(const Json::Value &cfg);
    virtual bool start();
    virtual bool stop();

public:
    virtual bool sendFrame(const char *buf, int len);
    virtual bool closeLink(int fd);

private:
    std::shared_ptr<base::CTcpClient> m_pTcpCli;
};

}

#endif /* __DATAENGINE_SRC_CHANNEL_TCPCLIENTCHANNEL_H__ */
