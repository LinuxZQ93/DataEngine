#ifndef __DATAENGINE_SRC_CHANNEL_CHANNELMANAGER_H__
#define __DATAENGINE_SRC_CHANNEL_CHANNELMANAGER_H__

#include "SingTon/SingTon.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "json/json.h"

#include "IChannel.h"
#include "IDataEngine.h"

namespace engine {

using fixFunc = TFunction<FIX_FRAME_VALUE, const std::string &, const char *, int, int&>;

class CChannelManager {
    SINGTON_DECLAR(CChannelManager)
public:
    bool createChannel(const std::vector<Json::Value> &vecCfgChannel);
    bool start();
    void attachFixFunc(const std::string &devName, const fixFunc &func);
    std::string getChannelName(const std::string &devName);

public:
    bool attachConnectStatusFunc(const std::string &name, const connectFunc &func);
    bool attachMsgAsyncFunc(const std::string &name, const msgFunc &func);
    bool closeLink(const std::string &name, int fd);
    bool sendFrame(const std::string &name, const char *buf, int len);
    bool recvFrame(const std::string &name, std::string &recvBuf, int timeOut);
    bool sendFrame(int fd, const std::string &name, const char *buf, int len);

private:
    bool createChannelPtr(const std::string &name, std::shared_ptr<IChannel> &ptr);
    void dealMsg(const std::string &devName, std::string &recvBuf);

private:
    std::map<std::string, std::shared_ptr<IChannel>> m_mapChannel;
    std::vector<std::shared_ptr<IChannel>> m_vecChannel;
    std::mutex m_mutexFunc;
    std::map<std::string, fixFunc> m_mapFixFunc;
};

}

#endif /* __DATAENGINE_SRC_CHANNEL_CHANNELMANAGER_H__ */
