#ifndef __FRAMEWORK_INCLUDE_THREAD_THREADMANAGER_H__
#define __FRAMEWORK_INCLUDE_THREAD_THREADMANAGER_H__

#include "IThreadManager.h"
#include "Component/ComponentMacroDef.h"

#include <thread>
#include <mutex>
#include <list>
#include <atomic>
#include <memory>

#include "Print/Print.h"
#include "SingTon/SingTon.h"
#include "Export/Export.h"

namespace base {

class ThreadImpl;
class DLL_EXPORT CThreadManager : public IThreadManager
{
    SIMPLE_DEF_C(ThreadManager, "ThreadManager")

public:
    virtual bool init();
    virtual bool start();
    virtual bool stop();
    virtual bool destroy();
    bool addThreadInfo(ThreadImpl* p);
    bool delThreadInfo(ThreadImpl* p);
private:
    void onConsole(const std::vector<std::string> &argList);
private:
    void loopThread();
private:
    ThreadImpl *m_threadOut;
    std::mutex m_mutex;
    std::list<ThreadImpl*> m_listThread;
    std::atomic<bool> m_bStart;
    std::atomic<bool> m_bInit;
    std::map<int, std::string> m_mapState;
};

}

#endif /* ifndef __FRAMEWORK_INCLUDE_THREAD_THREADMANAGER_H__ */

