#ifndef __FRAMEWORK_INCLUDE_AUTHENTICATION_AUTHENTICATIONRPC_H__
#define __FRAMEWORK_INCLUDE_AUTHENTICATION_AUTHENTICATIONRPC_H__

#include "Jsonrpc/IRpc.h"
#include "Jsonrpc/IRpcServer.h"
#include "SingTon/SingTon.h"
#include "Authentication.h"

namespace base {

class CAuthenticationRpc : public rpc::IRPC {
SINGTON_DECLAR(CAuthenticationRpc);
public:
    bool init();

private:
    void setLicense(const Json::Value &request, Json::Value &response);
    void getProtocol(const Json::Value &request, Json::Value &response);
private:
    CAuthentication *m_auth;
    rpc::IRpcServer *m_rpc;
    Json::Value m_cfgMsg;
};

}

#endif /* __FRAMEWORK_INCLUDE_AUTHENTICATION_AUTHENTICATIONRPC_H__ */
