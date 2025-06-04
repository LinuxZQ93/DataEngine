#ifndef __DATAENGINE_SRC_CHANNEL_UDPCHANNEL_H__
#define __DATAENGINE_SRC_CHANNEL_UDPCHANNEL_H__

#include "IChannel.h"

#include <memory>
#include "Channel/Net/Udp.h"

namespace engine {

class CUDPMsgHandle : public base::IHandleMessage, public CChannelMsgHandle {
public:
    CUDPMsgHandle();
    virtual ~CUDPMsgHandle();

public:
    virtual void handleMessage(int clientId, const char *buf, int len, std::string &response);
};

class CUDPChannel : public CChannelBase {
public:
    CUDPChannel();
    virtual ~CUDPChannel();

    virtual bool init(const Json::Value &cfg);
    virtual bool start();
    virtual bool stop();

public:
    virtual bool sendFrame(const char *buf, int len);
    virtual bool closeLink(int fd);

private:
    std::shared_ptr<base::CUdp> m_pUdp;
};

}

#endif /* __DATAENGINE_SRC_CHANNEL_UDPCHANNEL_H__ */
