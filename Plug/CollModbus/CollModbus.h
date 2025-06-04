#ifndef __DATAENGINEPLUG_COLLMODBUS_COLLMODBUS_H__
#define __DATAENGINEPLUG_COLLMODBUS_COLLMODBUS_H__

#include "DataEngine/IProtocol.h"
#include "Print/Print.h"

namespace engine {

class CCollModbus : public IProtocol
{
public:
    CCollModbus();
    virtual ~CCollModbus();
    SIMPLE_DEF_PROTOCOL(CCollModbus)
public:
    virtual bool init(const Json::Value &cfgValue);
    virtual bool process(const std::vector<std::string> &vecDeviceName);
    virtual FIX_FRAME_VALUE fixFrame(const std::string &devName, const char *buf, int len, int &validLen);
    virtual bool controlSelect(const std::string &devName, int pointId, CTRL_STATUS status);
    virtual bool controlExecute(const std::string &devName, int pointId, CTRL_STATUS status);
    virtual bool paramExecute(const std::string &devName, int transId, double data);
    virtual std::string getProtocolName();

};

}

#endif /* __DATAENGINEPLUG_COLLMODBUS_COLLMODBUS_H__ */
