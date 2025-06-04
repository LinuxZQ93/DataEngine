/**
*@file Export.h
*@brief 解析动态库常用宏定义
*
*Version:1.0
*
*Date:2020/09
*
*@author linuxzq93@163.com
*/
#ifndef __FRAMEWORK_INCLUDE_EXPORT_EXPORT_H__
#define __FRAMEWORK_INCLUDE_EXPORT_EXPORT_H__

#undef DLL_EXPORT
#undef API_CALL
#undef C_EXPORT

#define C_EXPORT extern "C"
#ifdef WIN32
#ifdef FM_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#define API_CALL __stdcall
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#define API_CALL
#endif

#define DLL_C_EXPORT C_EXPORT DLL_EXPORT

#endif /* ifndef __FRAMEWORK_INCLUDE_EXPORT_EXPORT_H__ */

