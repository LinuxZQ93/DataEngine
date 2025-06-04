#ifndef __FRAMEWORK_INCLUDE_CONSOLE_CONSOLE_H__
#define __FRAMEWORK_INCLUDE_CONSOLE_CONSOLE_H__

#include "IConsole.h"
#include "Component/ComponentMacroDef.h"

#include <string>
#include <map>
#include <mutex>

#include "SingTon/SingTon.h"
#include "Thread/Thread.h"

namespace base {

class CConsole : public IConsole
{
    SIMPLE_DEF_C(Console, "Console")
public:
    virtual bool init();
    virtual bool start();
    virtual bool stop();
public:
    virtual bool attach(const std::string &cmdName, const func &f);
    virtual bool detach(const std::string &cmdName);
    virtual bool query(std::vector<std::string> cmd);
private:
    void handleCmd();
    void notify();
    void loggin();
    void setPrintLevel(const std::vector<std::string> &argList);

private:
    bool                        m_bLogged;
    std::string                 m_strName;
    std::string                 m_strPasswd;
    std::mutex                  m_mutex;
    std::map<std::string, func> m_mapCmdName;
    ThreadWrap<void> m_thread;
    bool m_bRun;
};

}

#endif /* ifndef __FRAMEWORK_INCLUDE_CONSOLE_CONSOLE_H__ */

