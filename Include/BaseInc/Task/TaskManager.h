#ifndef __FRAMEWORK_INCLUDE_TASK_TASKMANAGER_H__
#define __FRAMEWORK_INCLUDE_TASK_TASKMANAGER_H__

#include "ITaskManager.h"

#include <atomic>
#include <string>
#include <memory>

#include "Thread/Thread.h"
#include "MsgQue/MsgQue.h"

namespace base {

class CTaskManager : public ITaskManager {
SIMPLE_DEF_C(TaskManager, "TaskManager")

public:
    virtual bool start();
    virtual bool stop();

public:
    virtual bool addTask(const Closure &funcWrap, const std::string &name);

private:
    ThreadPool<void> m_pool;
    std::atomic<bool> m_bStart;
};

}

#endif /* ifndef __FRAMEWORK_INCLUDE_TASK_TASKMANAGER_H__ */
