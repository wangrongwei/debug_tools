/*
 * logger.cc
 *
 *  Created on: 2018��12��7��
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
    /* ���캯�� */
    CLogger::CLogger(EnumLogLevel nLogLevel, const std::string strLogPath, const std::string strLogName)
        :m_nLogLevel(nLogLevel),m_strLogPath(strLogPath),m_strLogName(strLogName)
    {
        /* ��ʼ�� */
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
        //�����ļ���
        //MakeSureDirectoryPathExists(m_strLogPath.c_str());
        /* ������־�ļ� */
        if (m_strLogName.empty()){
            time_t curTime;
            time(&curTime);
            tm tm1;
#ifdef WIN
            localtime_s(&tm1, &curTime);
#elif LINUX
            /* ��LINUX�²���localtime_r���� */
            tm * tmp = localtime_r(&curTime,&tm1);
#endif
            //��־�������磺201601012130.log
            m_strLogName = FormatString("%04d%02d%02d_%02d%02d%02d.log",tm1.tm_year + 1900,tm1.tm_mon + 1,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);
        }
        m_strLogFilePath = m_strLogPath.append(m_strLogName);

#ifdef WIN
        //��׷�ӵķ�ʽ���ļ���
        fopen_s(&m_pFileStream, m_strLogFilePath.c_str(), "a+");
#elif LINUX
        m_pFileStream = fopen(m_strLogFilePath.c_str(), "a+");
#endif

        InitializeCriticalSection2(m_cs);
    }


    /* �������� */
    CLogger::~CLogger(){
        //�ͷ��ٽ���
        DeleteCriticalSection2(m_cs);
        //�ر��ļ���
        if (m_pFileStream){
            fclose(m_pFileStream);
            m_pFileStream = NULL;
        }
    }

    //�ļ�ȫ·���õ��ļ���
    const char *CLogger::path_file(const char *path, char splitter)
    {
        return strrchr(path, splitter) ? strrchr(path, splitter) + 1 : path;
    }

    //д���ش�����Ϣ
    void CLogger::TraceFatal(const char *lpcszFormat, ...)
    {
        //�жϵ�ǰ��д��־����
        if (EnumLogLevel::LogLevel_Fatal > m_nLogLevel){
            return;
        }
        string strResult;
        if (NULL != lpcszFormat){
            va_list marker;
            va_start(marker,lpcszFormat); //��ʼ����������
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //��ȡ��ʽ���ַ�������
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //���ñ�������
        }
        if (strResult.empty()){
            return;
        }
        string strLog = strFatalPrefix;
        strLog.append(GetTime()).append(strResult);

        //д��־�ļ�
        Trace(strLog);
    }

    //д������Ϣ
    void CLogger::TraceError(const char *lpcszFormat, ...)
    {
        //�жϵ�ǰ��д��־����
        if (EnumLogLevel::LogLevel_Error > m_nLogLevel)
            return;
        string strResult;
        if (NULL != lpcszFormat){
            va_list marker;
            va_start(marker,lpcszFormat); //��ʼ����������
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //��ȡ��ʽ���ַ�������
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //���ñ�������
        }
        if (strResult.empty()){
            return;
        }
        string strLog = strErrorPrefix;
        strLog.append(GetTime()).append(strResult);

        //д��־�ļ�
        Trace(strLog);
    }

    //д������Ϣ
    void CLogger::TraceWarning(const char *lpcszFormat, ...)
    {
        //�жϵ�ǰ��д��־����
        if (EnumLogLevel::LogLevel_Warning > m_nLogLevel)
            return;
        string strResult;
        if (NULL != lpcszFormat){
            va_list marker;
            va_start(marker,lpcszFormat); //��ʼ����������
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //��ȡ��ʽ���ַ�������
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //���ñ�������
        }
        if (strResult.empty()){
            return;
        }
        string strLog = strWarningPrefix;
        strLog.append(GetTime()).append(strResult);

        //д��־�ļ�
        Trace(strLog);
    }

    /* дһ����Ϣ */
    void CLogger::TraceInfo(const char *lpcszFormat, ...)
    {
        //�жϵ�ǰ��д��־����
        if (EnumLogLevel::LogLevel_Info > m_nLogLevel){
            return;
        }
        string strResult;
        if (NULL != lpcszFormat){
            va_list marker;
            va_start(marker,lpcszFormat); //��ʼ����������
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //��ȡ��ʽ���ַ�������
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //���ñ�������
        }
        if (strResult.empty()){
            return;
        }
        string strLog = strInfoPrefix;
        strLog.append(GetTime()).append(strResult);

        //д��־�ļ�
        Trace(strLog);
    }

    /* ��ȡϵͳ��ǰʱ�� */
    string CLogger::GetTime()
    {
        time_t curTime;
        time(&curTime);
        tm tm1;
#ifdef WIN
            localtime_s(&tm1, &curTime);
#elif LINUX
            /* ��LINUX�²���localtime_r���� */
            tm * tmp = localtime_r(&curTime,&tm1);
#endif
        //2016-01-01 21:30:00
        string strTime = FormatString("%04d-%02d-%02d %02d:%02d:%02d ", tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

        return strTime;
    }

    //�ı�д��־����
    void CLogger::ChangeLogLevel(EnumLogLevel nLevel)
    {
        m_nLogLevel = nLevel;
    }

    /* д�ļ����� */
    void CLogger::Trace(const string &strLog)
    {
        try{
            //�����ٽ���
            EnterCriticalSection2(m_cs);
            //���ļ���û�д򿪣������´�
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
            //д��־��Ϣ���ļ���
            fprintf(m_pFileStream, "%s\n", strLog.c_str());
            fflush(m_pFileStream);
            //�뿪�ٽ���
            LeaveCriticalSection2(m_cs);
        }
        //�������쳣�������뿪�ٽ�������ֹ����
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
            va_start(marker,lpcszFormat); //��ʼ����������
#ifdef WIN
            size_t nLength = _vscprintf(lpcszFormat,marker) + 1; //��ȡ��ʽ���ַ�������
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = _vsnprintf_s(&vBuffer[0],vBuffer.size(),nLength,lpcszFormat,marker);
#elif LINUX
            size_t nLength = vsnprintf(NULL,0,lpcszFormat,marker) + 1;
            std::vector<char> vBuffer(nLength,'\0'); //�������ڴ洢��ʽ���ַ������ַ�����
            int nWritten = vsnprintf(&vBuffer[0],vBuffer.size(),lpcszFormat,marker);
#endif
            if (nWritten > 0){
                strResult = &vBuffer[0];
            }
            va_end(marker); //���ñ�������
        }
        return strResult;
    }

    /* ������� */
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

    /* ��ʼ���� */
    void CLogger::InitializeCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs)
    {
#ifdef WIN
        InitializeCriticalSection(&m_cs);
#elif LINUX
        pthread_mutex_init(&m_cs,NULL);
#endif
    }
    /* ɾ���� */
    void CLogger::DeleteCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs)
    {
#ifdef WIN
        DeleteCriticalSection(&m_cs);
#elif LINUX
        pthread_mutex_destroy(&m_cs);
#endif
    }
    /* �����ٽ��� */
    void CLogger::EnterCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs)
    {
#ifdef WIN
        EnterCriticalSection(&m_cs);
#elif LINUX
        pthread_mutex_lock(&m_cs);
#endif
    }
    /* �뿪�ٽ��� */
    void CLogger::LeaveCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs)
    {
#ifdef WIN
        LeaveCriticalSection(&m_cs);
#elif LINUX
        pthread_mutex_unlock(&m_cs);
#endif
    }

}



