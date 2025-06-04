/**
*@file SingTon.h
*@brief 单例宏，方便定义单例类
*
*Version:1.0
*
*Date:2020/07
*
*@author linuxzq93@163.com
*/
#ifndef __FRAMEWORK_INCLUDE_SINGTON_SINGTON_H__
#define __FRAMEWORK_INCLUDE_SINGTON_SINGTON_H__

#include <mutex>
#include <string>
// #include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#undef SINGTON_DECLAR
#define SINGTON_DECLAR(className) \
public:\
    static className *instance();\
private:\
    className();\
    ~className();\

#undef SINGTON_DEFINITION
#define SINGTON_DEFINITION(className) \
className *className::instance() \
{\
    static std::mutex s_mutex;\
    static className *s_instance;\
    if (nullptr == s_instance) \
    {\
        std::lock_guard<std::mutex> Guard(s_mutex);\
        if (nullptr == s_instance) \
        {\
            s_instance = new className;\
        }\
    } \
    return s_instance;\
}

#if 0
#undef SINGTON_DEFINITIONSHARE
#define SINGTON_DEFINITIONSHARE(className) \
className *className::instance() \
{\
    static std::mutex s_mutex;\
    static className *s_instance;\
    if (nullptr == s_instance) \
    {\
        bool bMain = false;\
        char exe[128] = {0};\
        readlink("/proc/self/exe", exe, sizeof(exe) - 1);\
        std::string str(exe);\
        if (str.find("sophia") != std::string::npos)\
        { \
            bMain = true;\
        }\
        std::lock_guard<std::mutex> Guard(s_mutex);\
        if (nullptr == s_instance) \
        {\
            int shmFlags = O_CREAT | O_RDWR; \
            mode_t shmMode = (S_IRUSR | S_IWUSR);\
            int shmFd = shm_open(#className, shmFlags, shmMode);\
            if (shmFd < 0) \
            {\
                return nullptr; \
            }\
            if (ftruncate(shmFd, sizeof(className)) < 0)\
            {\
                return nullptr;\
            }\
            void *ptr = mmap(NULL, sizeof(className), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);\
            infof("mmap %s, ptr %p\n", #className, ptr);\
            if (ptr == MAP_FAILED) \
            {\
                return nullptr;\
            }\
            if (bMain)\
            {\
                s_instance = new (ptr) className;\
            }\
            else\
            {\
                s_instance = (className *)ptr;\
            }\
            close(shmFd);\
        }\
    } \
    return s_instance;\
}
#endif

#endif /* ifndef __FRAMEWORK_INCLUDE_SINGTON_SINGTON_H__ */

