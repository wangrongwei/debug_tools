/*
 * logger.cc
 *
 *  Created on: 2018年12月7日
 *      Author: lollipop
 */

//logger.cpp
#include <vector>
#include "logger.h"
#include <stdarg.h>
#include <string.h>

#ifdef WIN
#include <time.h>
#include <direct.h>
#include <Dbghelp.h>
#include <_dbg_common.h>
#elif LINUX
#include <limits.h>
#include <time.h>
#include <libgen.h>
#define MAX_PATH PATH_MAX
#endif

using std::string;
using std::vector;

namespace LOGGER
{
    /* 构造函数 */
    CLogger::CLogger(EnumLogLevel nLogLevel, const std::string strLogPath, const std::string strLogName)
        :m_nLogLevel(nLogLevel),m_strLogPath(strLogPath),m_strLogName(strLogName)
    {
        /* 初始化 */
        m_pFileStream = NULL;
        if (m_strLogPath.empty()){
            m_strLogPath = GetAppPathA();
        }
#ifdef WIN
        if (m_strLogPath[m_strLogPath.length()-1] != '\\'){
            m_strLogPath.append("\\");
        }
#elif LINUX
        if (m_strLogPath[m_strLogPath.length()-1] != '/'){
            m_strLogPath.append("/");
        }
#endif
        //创建文件夹
        //MakeSureDirectoryPathExists(m_strLogPath.c_str());
        /* 创建日志文件 */
        if (m_strLogName.empty()){
            time_t curTime;
            time(&curTime);
            tm tm1;
#ifdef WIN
            localtime_s(&tm1, &curTime);
#elif LINUX
            /* 在LINUX下采用localtime_r代替 */
            tm * tmp = localtime_r(&curTime,&tm1);
#endif
            //日志的名称如：201601012130.log
            m_strLogName = FormatString("%04d%02d%02d_%02d%02d%02d.log",tm1.tm_year + 1900,tm1.tm_mon + 1,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);
        }
        m_strLogFilePath = m_strLogPath.append(m_strLogName);

#ifdef WIN
        //以追加的方式打开文件流
        fopen_s(&m_pFileStream, m_strLogFilePath.c_str(), "a+");
#elif LINUX
        m_pFileStream = fopen(m_strLogFilePath.c_str(), "a+");
#endif

        InitializeCriticalSection2(m_cs);
    }


    /* 析构函数 */
    CLogger::~CLogger(){
        //释放临界区
        DeleteCriticalSection2(m_cs);
        //关闭文件流
        if (m_pFileStream){
            fclose(m_pFileStream);
            m_pFileStream = NULL;
        }
    }

    //文件全路径得到文件名
    const char *CLogger::path_file(const char *path, char splitter)
    {
        return strrchr(path, splitter) ? strrchr(path, splitter) + 1 : path;
    }

    //写严重错误信息
    void CLogger::TraceFatal(const char *lpcszFormat, ...)
    {
        //判断当前的写日志级别
        if (EnumLogLevel::LogLevel_Fatal > m_nLogLevel){
            return;
        }
        string strResult;
        if (NULL != lpcszFormat){
            va_list marker;
            va_start(marker,lpcszFormat); //初始化变量参数
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //获取格式化字符串长度
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //重置变量参数
        }
        if (strResult.empty()){
            return;
        }
        string strLog = strFatalPrefix;
        strLog.append(GetTime()).append(strResult);

        //写日志文件
        Trace(strLog);
    }

    //写错误信息
    void CLogger::TraceError(const char *lpcszFormat, ...)
    {
        //判断当前的写日志级别
        if (EnumLogLevel::LogLevel_Error > m_nLogLevel)
            return;
        string strResult;
        if (NULL != lpcszFormat){
            va_list marker;
            va_start(marker,lpcszFormat); //初始化变量参数
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //获取格式化字符串长度
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //重置变量参数
        }
        if (strResult.empty()){
            return;
        }
        string strLog = strErrorPrefix;
        strLog.append(GetTime()).append(strResult);

        //写日志文件
        Trace(strLog);
    }

    //写警告信息
    void CLogger::TraceWarning(const char *lpcszFormat, ...)
    {
        //判断当前的写日志级别
        if (EnumLogLevel::LogLevel_Warning > m_nLogLevel)
            return;
        string strResult;
        if (NULL != lpcszFormat){
            va_list marker;
            va_start(marker,lpcszFormat); //初始化变量参数
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //获取格式化字符串长度
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //重置变量参数
        }
        if (strResult.empty()){
            return;
        }
        string strLog = strWarningPrefix;
        strLog.append(GetTime()).append(strResult);

        //写日志文件
        Trace(strLog);
    }

    /* 写一般信息 */
    void CLogger::TraceInfo(const char *lpcszFormat, ...)
    {
        //判断当前的写日志级别
        if (EnumLogLevel::LogLevel_Info > m_nLogLevel){
            return;
        }
        string strResult;
        if (NULL != lpcszFormat){
            va_list marker;
            va_start(marker,lpcszFormat); //初始化变量参数
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //获取格式化字符串长度
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //重置变量参数
        }
        if (strResult.empty()){
            return;
        }
        string strLog = strInfoPrefix;
        strLog.append(GetTime()).append(strResult);

        //写日志文件
        Trace(strLog);
    }

    /* 获取系统当前时间 */
    string CLogger::GetTime()
    {
        time_t curTime;
        time(&curTime);
        tm tm1;
#ifdef WIN
            localtime_s(&tm1, &curTime);
#elif LINUX
            /* 在LINUX下采用localtime_r代替 */
            tm * tmp = localtime_r(&curTime,&tm1);
#endif
        //2016-01-01 21:30:00
        string strTime = FormatString("%04d-%02d-%02d %02d:%02d:%02d ", tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

        return strTime;
    }

    //改变写日志级别
    void CLogger::ChangeLogLevel(EnumLogLevel nLevel)
    {
        m_nLogLevel = nLevel;
    }

    /* 写文件操作 */
    void CLogger::Trace(const string &strLog)
    {
        try{
            //进入临界区
            EnterCriticalSection2(m_cs);
            //若文件流没有打开，则重新打开
            if (NULL == m_pFileStream){
#ifdef WIN
                fopen_s(&m_pFileStream, m_strLogFilePath.c_str(), "a+");
#elif LINUX
                m_pFileStream = fopen(m_strLogFilePath.c_str(), "a+");
#endif
                if (!m_pFileStream){
                    return;
                }
            }
            //写日志信息到文件流
            fprintf(m_pFileStream, "%s\n", strLog.c_str());
            fflush(m_pFileStream);
            //离开临界区
            LeaveCriticalSection2(m_cs);
        }
        //若发生异常，则先离开临界区，防止死锁
        catch (...){
            LeaveCriticalSection2(m_cs);
        }

    }

    string CLogger::GetAppPathA()
    {
        char szFilePath[MAX_PATH] = { 0 }, szDrive[MAX_PATH] = { 0 }, szDir[MAX_PATH] = { 0 }, szFileName[MAX_PATH] = { 0 }, szExt[MAX_PATH] = { 0 };
#ifdef WIN
        GetModuleFileNameA(NULL, szFilePath, sizeof(szFilePath));
        _splitpath_s(szFilePath, szDrive, szDir, szFileName, szExt);

        std::string str(szDrive);
        str.append(szDir);
#elif LINUX
        FILE* stream = fopen("/proc/self/cmdline","r");
        fgets(szFilePath,sizeof(szFilePath),stream);
        fclose(stream);
        std::string str(dirname(szFilePath));
#endif
        return str;
    }

    string CLogger::FormatString(const char *lpcszFormat, ...)
    {
        string strResult;
        if (NULL != lpcszFormat){
            va_list marker;
            va_start(marker,lpcszFormat); //初始化变量参数
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //获取格式化字符串长度
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //创建用于存储格式化字符串的字符数组
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //重置变量参数
        }
        return strResult;
    }

    /* 后期添加 */
    CLogger & operator<<(CLogger &logout, const char *str)
    {
        logout.out << str;
        logout.Trace(logout.out.str());
         //Trace(const string &strLog);
        return logout;
    }
    CLogger & operator>>(CLogger &login, const char &obj)
    {
        //login.in >> obj;
        //if (!in){
            //obj = Test();
        //}
        return login;
    }

    /* 初始化锁 */
    void CLogger::InitializeCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs)
    {
#ifdef WIN
        InitializeCriticalSection(&m_cs);
#elif LINUX
        pthread_mutex_init(&m_cs,NULL);
#endif
    }
    /* 删除锁 */
    void CLogger::DeleteCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs)
    {
#ifdef WIN
        DeleteCriticalSection(&m_cs);
#elif LINUX
        pthread_mutex_destroy(&m_cs);
#endif
    }
    /* 进入临界区 */
    void CLogger::EnterCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs)
    {
#ifdef WIN
        EnterCriticalSection(&m_cs);
#elif LINUX
        pthread_mutex_lock(&m_cs);
#endif
    }
    /* 离开临界区 */
    void CLogger::LeaveCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs)
    {
#ifdef WIN
        LeaveCriticalSection(&m_cs);
#elif LINUX
        pthread_mutex_unlock(&m_cs);
#endif
    }

}



