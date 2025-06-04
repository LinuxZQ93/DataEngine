#ifndef __FRAMEWORK_INCLUDE_CHANNEL_NET_UDP_H__
#define __FRAMEWORK_INCLUDE_CHANNEL_NET_UDP_H__

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#endif

#include <memory>

#include "Channel/IHandleMessage.h"
#include "Export/Export.h"
#include "Thread/Thread.h"

namespace base {

class DLL_EXPORT CUdp {
    static const int sm_maxRecv;
public:
    CUdp(const std::string &localIp, int localPort, const std::string &remoteIp, int remotePort, const std::shared_ptr<IHandleMessage> &pHandle);
    ~CUdp();

public:
    bool start();
    bool stop();
    bool send(const char *buf, int len) {if (m_socketFd < 0) return false; else return ::sendto(m_socketFd, buf, len, 0, (struct sockaddr *)&m_addrOut, sizeof(m_addrOut));}
    int getSocketFd() {return m_socketFd;}

private:
    bool initServer();
    void listenLoop();
    void handleRequest(fd_set &fs, char *buf);

private:
    base::ThreadWrap<void> m_thread;
    std::shared_ptr<IHandleMessage> m_pHandle;
    int m_socketFd;
    struct sockaddr_in m_addrIn;
    struct sockaddr_in m_addrOut;
    std::string m_localIp;
    int m_localPort;
    std::string m_remoteIp;
    int m_remotePort;
};

}

#endif /* __FRAMEWORK_INCLUDE_CHANNEL_NET_UDP_H__ */
