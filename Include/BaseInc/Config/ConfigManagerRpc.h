#ifndef __FRAMEWORK_INCLUDE_CONFIG_CONFIGMANAGERRPC_H__
#define __FRAMEWORK_INCLUDE_CONFIG_CONFIGMANAGERRPC_H__

#include "Jsonrpc/IRpcServer.h"
#include "Jsonrpc/IRpc.h"
#include "IConfigManager.h"
#include "SingTon/SingTon.h"

namespace base {

class CConfigManagerRpc : public rpc::IRPC {
private:
    SINGTON_DECLAR(CConfigManagerRpc);
public:
    bool init();
    void setConfig(const Json::Value &request, Json::Value &response);
    void getConfig(const Json::Value &request, Json::Value &response);

private:
    IConfigManager *m_configManager;
    rpc::IRpcServer *m_rpc;
    Json::Value m_cfgMsg;
};
}

#endif /* ifndef __FRAMEWORK_INCLUDE_CONFIG_CONFIGMANAGERRPC_H__ */

