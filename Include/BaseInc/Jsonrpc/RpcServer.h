#ifndef __FRAMEWORK_INCLUDE_JSONRPC_RPCSERVER_H__
#define __FRAMEWORK_INCLUDE_JSONRPC_RPCSERVER_H__

#include "IRpcServer.h"
#include "Component/ComponentMacroDef.h"

#include <map>
#include <string>
#include <mutex>

#include "jsonrpccpp/common/procedure.h"
#include "jsonrpccpp/server/abstractserverconnector.h"
#include "jsonrpccpp/server/iprocedureinvokationhandler.h"
#include "jsonrpccpp/server/requesthandlerfactory.h"

#include "Config/IConfigManager.h"

#include "Export/Export.h"

namespace rpc {

class DLL_EXPORT CRpcServer : public jsonrpc::IProcedureInvokationHandler, public IRpcServer
{
    SIMPLE_DEF_C(RpcServer, "RPCServer")
public:
    virtual void HandleMethodCall(jsonrpc::Procedure &proc, const Json::Value& input, Json::Value& output);
    virtual void HandleNotificationCall(jsonrpc::Procedure &proc, const Json::Value& input);

public:
    virtual bool init();
    virtual bool start();
    virtual bool stop();
    virtual bool destroy();
    virtual bool attachFunc(const std::string &funcName, const jsonCompleteType &type, const Func &func);
    virtual bool attachNotifyFunc(const std::string &funcName, const jsonCompleteType &type, const FuncNotify &func);
    virtual bool attachCheckFunc(const TFunction<bool, const Json::Value&, Json::Value&> &func) { m_checkFunc = func; return true;}

private:
    bool symbolExists(const std::string &name);

private:
    jsonrpc::AbstractServerConnector    *m_connector;
    jsonrpc::IProtocolHandler           *m_handler;
    base::IConfigManager *m_configManager;
    std::map<std::string, Func>         m_mapFunc;
    std::map<std::string, FuncNotify>   m_mapFuncNotify;
    TFunction<bool, const Json::Value&, Json::Value&> m_checkFunc;
};

}

#endif /* ifndef __FRAMEWORK_INCLUDE_JSONRPC_RPCSERVER_H__ */

