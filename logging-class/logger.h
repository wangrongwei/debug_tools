/*
 * logger.h
 *
 *  Created on: 2018��12��7��
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
/* �ٽ������� */
typedef CRITICAL_SECTION MUTEX_OR_CRITICAL_SECTION;
#elif LINUX
#include <pthread.h>
/* �ٽ������� */
typedef pthread_mutex_t MUTEX_OR_CRITICAL_SECTION;
#endif

using namespace std;

namespace LOGGER
{
    //��־�������ʾ��Ϣ
    static const std::string strFatalPrefix = "Fatal\t";
    static const std::string strErrorPrefix = "Error\t";
    static const std::string strWarningPrefix = "Warning\t";
    static const std::string strInfoPrefix = "Info\t";

    //��־����ö��
    typedef enum EnumLogLevel{
        LogLevel_Stop = 0,  //ʲô������¼
        LogLevel_Fatal,     //ֻ��¼���ش���
        LogLevel_Error,     //��¼���ش�����ͨ����
        LogLevel_Warning,   //��¼���ش�����ͨ���󣬾���
        LogLevel_Info       //��¼���ش�����ͨ���󣬾��棬��ʾ��Ϣ(Ҳ����ȫ����¼)
    };

    class CLogger
    {
    public:
        friend CLogger & operator << (CLogger &out, const char *str);
        friend CLogger & operator >> (CLogger &in, const char &obj);
        //nLogLevel����־��¼�ĵȼ����ɿ�
        //strLogPath����־Ŀ¼���ɿ�
        //strLogName����־���ƣ��ɿ�
        CLogger(EnumLogLevel nLogLevel = EnumLogLevel::LogLevel_Info, const std::string strLogPath = "", const std::string strLogName = "");
        //��������
        virtual ~CLogger();
    public:
        ostringstream out;
        void TraceFatal(const char *lpcszFormat, ...); /* д���ش�����Ϣ */
        void TraceError(const char *lpcszFormat, ...); /* д������Ϣ */
        void TraceWarning(const char *lpcszFormat, ...); /* д������Ϣ */
        void TraceInfo(const char *lpcszFormat, ...); /* д��ʾ��Ϣ */
        void ChangeLogLevel(EnumLogLevel nLevel); /* �ı�д��־���� */
        static std::string GetAppPathA(); /* ��ȡ��������·�� */
        static std::string FormatString(const char *lpcszFormat, ...); /* ��ʽ���ַ��� */

        /* ��ʼ���ٽ��� */
        void InitializeCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs);
        void DeleteCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs);
        void EnterCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs);
        void LeaveCriticalSection2(MUTEX_OR_CRITICAL_SECTION m_cs);

    private:
        void Trace(const std::string &strLog); /* д�ļ����� */
        std::string GetTime(); /* ��ȡ��ǰϵͳʱ�� */
        const char *path_file(const char *path, char splitter); /* �ļ�ȫ·���õ��ļ��� */
    private:
        FILE * m_pFileStream; /* д��־�ļ��� */
        EnumLogLevel m_nLogLevel; /* д��־���� */
        std::string m_strLogPath; /* ��־Ŀ¼ */
        std::string m_strLogName; /* ��־������ */
        std::string m_strLogFilePath; /* ��־�ļ�ȫ·�� */

        /* �߳�ͬ�����ٽ������� */
        MUTEX_OR_CRITICAL_SECTION m_cs;
    };
}

#endif

