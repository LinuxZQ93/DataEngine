#ifndef __FRAMEWORK_INCLUDE_CONSOLE_CONSOLERPC_H__
#define __FRAMEWORK_INCLUDE_CONSOLE_CONSOLERPC_H__

#include "Jsonrpc/IRpc.h"
#include "Jsonrpc/IRpcServer.h"
#include "IConsole.h"
#include "SingTon/SingTon.h"

namespace base {

class CConsoleRpc : public rpc::IRPC {
private:
    SINGTON_DECLAR(CConsoleRpc);
public:
    virtual bool init();
    void queryCmd(const Json::Value &request, Json::Value &response);

private:
    IConsole *m_console;
    rpc::IRpcServer *m_rpc;
};
}

#endif /* ifndef __FRAMEWORK_INCLUDE_CONSOLE_CONSOLERPC_H__ */

