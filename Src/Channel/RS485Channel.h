#ifndef __DATAENGINE_SRC_CHANNEL_RS485CHANNEL_H__
#define __DATAENGINE_SRC_CHANNEL_RS485CHANNEL_H__

#include "IChannel.h"

#include "Channel/Serial/Serial.h"
#include <memory>

namespace engine {

class CRS485MsgHandle : public base::IHandleMessage, public CChannelMsgHandle {
public:
    CRS485MsgHandle();
    virtual ~CRS485MsgHandle();

public:
    virtual void handleMessage(int clientId, const char *buf, int len, std::string &response);
};

class CRS485Channel : public CChannelBase {
public:
    CRS485Channel();
    virtual ~CRS485Channel();

    virtual bool init(const Json::Value &cfg);
    virtual bool start();
    virtual bool stop();

public:
    virtual bool sendFrame(const char *buf, int len);
    virtual bool closeLink(int fd);

private:
    std::shared_ptr<base::CSerial> m_pSerial;
};

}

#endif /* __DATAENGINE_SRC_CHANNEL_RS485CHANNEL_H__ */