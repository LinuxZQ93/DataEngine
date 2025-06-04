#ifndef __DATAENGINE_SRC_PROTOCOL_IPROCOTOL_H__
#define __DATAENGINE_SRC_PROTOCOL_IPROCOTOL_H__

#include <vector>
#include <string>
#include <mutex>
#include "Export/Export.h"
#include "json/json.h"

#include "IDataEngine.h"

#undef SIMPLE_DEF_PROTOCOL

#define SIMPLE_DEF_PROTOCOL(ProtocolName) \
public: \
    class ProtocolName##ProtocolFactory : public IProtocolFactory { \
    public: \
        ProtocolName##ProtocolFactory(const std::string &taskName) : IProtocolFactory(taskName) { \
            m_protocol = new ProtocolName(); \
            if (!registerProtocol(taskName)) { \
                warnf("register protocol %s failed\n", taskName.c_str());\
            } \
            else { \
                infof("register protocol %s success\n", taskName.c_str());\
            } \
        } \
        virtual ~ProtocolName##ProtocolFactory() { } \
    };

namespace engine {

class DLL_EXPORT IProtocol {
public:
    IProtocol();
    virtual ~IProtocol();
public:
    class DLL_EXPORT IProtocolFactory {
    public:
        IProtocolFactory(const std::string &taskName);
        virtual ~IProtocolFactory();
        bool registerProtocol(const std::string &taskName);
    protected:
        IProtocol *m_protocol;
    };
public:
    virtual bool init(const Json::Value &cfgValue) = 0;
    virtual bool process(const std::vector<std::string> &vecDeviceName) = 0;
    virtual FIX_FRAME_VALUE fixFrame(const std::string &devName, const char *buf, int len, int &validLen) = 0;
    virtual bool controlSelect(const std::string &devName, int pointId, CTRL_STATUS status){return true;}
    virtual bool controlExecute(const std::string &name, int transId, CTRL_STATUS status) {return true;}
    virtual bool controlCancel(const std::string &name, int transId, CTRL_STATUS status){return true;}
    virtual bool paramSelect(const std::string &name, int transId, double data){return true;}
    virtual bool paramExecute(const std::string &name, int transId, double data){return true;}
    virtual bool paramCancel(const std::string &name, int transId, double data){return true;}
    virtual std::string getProtocolName() = 0;

public:
    void setLockSource(std::mutex *mutex){m_mutex = mutex;}

protected:
    IDataEngine *m_pDataEngine;
    std::mutex *m_mutex;
};

}

#endif /* __DATAENGINE_SRC_PROTOCOL_IPROCOTOL_H__ */
