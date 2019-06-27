/*
 * logger.h
 *
 *  Created on: 2018年12月7日
 *      Author: lollipop
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <strstream>

#define LINUX 1

#ifdef WIN
#include <Windows.h>
/* 临界区变量 */
typedef CRITICAL_SECTION MUTEX_OR_CRITICAL_SECTION;
#elif LINUX
#include <pthread.h>
/* 临界区变量 */
typedef pthread_mutex_t MUTEX_OR_CRITICAL_SECTION;
#endif

using namespace std;

namespace LOGGER
{
    //日志级别的提示信息
    static const std::string strFatalPrefix = "Fatal\t";
    static const std::string strErrorPrefix = "Error\t";
    static const std::string strWarningPrefix = "Warning\t";
    static const std::string strInfoPrefix = "Info\t";

    //日志级别枚举
    typedef enum EnumLogLevel{
        LogLevel_Stop = 0,  //什么都不记录
        LogLevel_Fatal,     //只记录严重错误
        LogLevel_Error,     //记录严重错误，普通错误
        LogLevel_Warning,   //记录严重错误，普通错误，警告
        LogLevel_Info       //记录严重错误，普通错误，警告，提示信息(也就是全部记录)
    };

    class CLogger
    {
    public:
        friend CLogger & operator << (CLogger &out, const char *str);
        friend CLogger & operator >> (CLogger &in, const char &obj);
        //nLogLevel：日志记录的等级，可空
        //strLogPath：日志目录，可空
        //strLogName：日志名称，可空
        CLogger(EnumLogLevel nLogLevel = EnumLogLevel::LogLevel_Info, const std::string strLogPath = "", const std::string strLogName = "");
        //析构函数
        virtual ~CLogger();
    public:
        ostringstream out;
        void TraceFatal(const char *lpcszFormat, ...); /* 写严重错误信息 */
        void TraceError(const char *lpcszFormat, ...); /* 写错误信息 */
        void TraceWarning(const char *lpcszFormat, ...); /* 写警告信息 */
        void TraceInfo(const char *lpcszFormat, ...); /* 写提示信息 */
        void ChangeLogLevel(EnumLogLevel nLevel); /* 改变写日志级别 */
        static std::string GetAppPathA(); /* 获取程序运行路径 */
        static std::string FormatString(const char *lpcszFormat, ...); /* 格式化字符串 */

        /* 初始化临界区 */
        void InitializeCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs);
        void DeleteCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs);
        void EnterCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs);
        void LeaveCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs);

    private:
        void Trace(const std::string &strLog); /* 写文件操作 */
        std::string GetTime(); /* 获取当前系统时间 */
        const char *path_file(const char *path, char splitter); /* 文件全路径得到文件名 */
    private:
        FILE * m_pFileStream; /* 写日志文件流 */
        EnumLogLevel m_nLogLevel; /* 写日志级别 */
        std::string m_strLogPath; /* 日志目录 */
        std::string m_strLogName; /* 日志的名称 */
        std::string m_strLogFilePath; /* 日志文件全路径 */

        /* 线程同步的临界区变量 */
        MUTEX_OR_CRITICAL_SECTION m_cs;
    };
}

#endif

