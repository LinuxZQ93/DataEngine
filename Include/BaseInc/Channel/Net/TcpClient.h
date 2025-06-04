#ifndef __FRAMEWORK_INCLUDE_CHANNEL_NET_TCPCLIENT_H__
#define __FRAMEWORK_INCLUDE_CHANNEL_NET_TCPCLIENT_H__

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
#include "Thread/Thread.h"

namespace base {

class DLL_EXPORT CTcpClient {
    static const int sm_maxRecv;
public:
    CTcpClient(const std::string &ip, int port, const std::shared_ptr<ITcpClientHandleMessage> &Ihandle);
    ~CTcpClient();

public:
    bool start(bool bReconnect = true);
    bool stop();
    bool send(const char *buf, int len) {if (!m_bConnect || m_socketFd < 0) return false; else return ::send(m_socketFd, buf, len, 0);}
    bool getConnectState() {return m_bConnect;}
    int getSocketFd() {return m_socketFd;}

private:
    void reconnect();
    bool connect();
    void loop();
    void handleRequest(fd_set &fs, char *buf);

private:
    std::string m_strIP;
    int m_port;
    int m_socketFd;
    std::shared_ptr<ITcpClientHandleMessage> m_pHandle;
    base::ThreadWrap<void> m_thread;
    bool m_bStart;
    bool m_bConnect;
    base::ThreadWrap<void> m_threadRe;
};

}

#endif /* __FRAMEWORK_INCLUDE_CHANNEL_NET_CLIENT_H__ */
