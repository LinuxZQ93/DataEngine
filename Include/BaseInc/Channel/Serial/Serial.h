#ifndef __FRAMEWORK_INCLUDE_CHANNEL_SERIAL_SERIAL_H__
#define __FRAMEWORK_INCLUDE_CHANNEL_SERIAL_SERIAL_H__

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

class DLL_EXPORT CSerial {
public:
    enum BAUDRATE {
        COM_B1200 = 0,
        COM_B2400,
        COM_B4800,
        COM_B9600,
        COM_B19200,
        COM_B38400,
        COM_B57600,
        COM_B115200,
        COM_B300,
        COM_B600,
    };

    enum DATABITS {
        COM_D5 = 0,
        COM_D6,
        COM_D7,
        COM_D8
    };

    enum PARITY {
        COM_NOME = 0,
        COM_ODD,
        COM_EVEN
    };

    enum STOPBITS {
        COM_S0 = 0,
        COM_S1,
        COM_S2
    };

    struct Config {
        std::string portName;
        BAUDRATE baudrate;
        DATABITS databits;
        STOPBITS stopbits;
        PARITY parity;
    };

    static const int sm_maxRecv;
public:
    CSerial(const Config &config, const std::shared_ptr<IHandleMessage> &handler);
    ~CSerial();

public:
    bool start();
    bool stop();
    bool send(const char *buf, int len) {if (m_fd < 0) return false; else return ::send(m_fd, buf, len, 0);}
    int getSocketFd() {return m_fd;}

private:
    bool initSerial();
    int getBaudrate(int baud);
    int getDatabits(int databits);
    void loop();
    void handleRequest(fd_set &fs, char *buf);

private:
    int m_fd;
    Config m_cfg;
    std::shared_ptr<IHandleMessage> m_pHandle;
    base::ThreadWrap<void> m_thread;
};

}

#endif /* __FRAMEWORK_INCLUDE_CHANNEL_SERIAL_RS485_H__ */
