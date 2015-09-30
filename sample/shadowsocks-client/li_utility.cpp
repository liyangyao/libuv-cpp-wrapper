#include "li_utility.h"
#include <QWaitCondition>
#include <QStringList>
#include <Windows.h>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")

namespace Utility{

QString getModuleName()
{
    wchar_t buffer[MAX_PATH + 2];
    DWORD v = GetModuleFileName(0, buffer, MAX_PATH + 1);
    buffer[MAX_PATH + 1] = 0;

    if (v == 0)
        return QString();
    else
        return QString::fromWCharArray(buffer);
}

QString getHostName()
{
    wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 2];
    DWORD size = MAX_COMPUTERNAME_LENGTH;
    DWORD v = GetComputerName(buffer, &size);
    buffer[MAX_COMPUTERNAME_LENGTH + 1] = 0;

    if (v == 0)
        return QString();
    else
        return QString::fromWCharArray(buffer);
}

const char *formatShortTime_c()
{
    SYSTEMTIME lpsystime;
    GetLocalTime(&lpsystime);
    static char buff[32];
    _snprintf_s(buff, sizeof(buff), "%02u%02u%02u%02u%02u",lpsystime.wMonth,
                lpsystime.wDay,lpsystime.wHour,lpsystime.wMinute,lpsystime.wSecond);
    return buff;
}

QString formatShortTime()
{
    return QString(formatShortTime_c());
}

LONG WINAPI myUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
    wchar_t szProgramPath[MAX_PATH + 2] = {0};
    if(GetModuleFileName(NULL, szProgramPath, MAX_PATH))
    {
        LPTSTR lpSlash = wcsrchr(szProgramPath, '\\');
        if(lpSlash)
        {
            *(lpSlash + 1) = '\0';
        }
    }
    wchar_t szDumpFile[MAX_PATH] = {0};
    swprintf_s(szDumpFile, MAX_PATH, L"%s%s.dmp", szProgramPath, formatShortTime_c());

    HANDLE lhDumpFile = CreateFile(szDumpFile, GENERIC_WRITE, 0, NULL,
                                   CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
    MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
    loExceptionInfo.ExceptionPointers = ExceptionInfo;
    loExceptionInfo.ThreadId = GetCurrentThreadId();
    loExceptionInfo.ClientPointers = true;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile,
                      MiniDumpWithIndirectlyReferencedMemory, &loExceptionInfo,
                      NULL, NULL);
    CloseHandle(lhDumpFile);

    return EXCEPTION_EXECUTE_HANDLER;
}

void installMiniDump()
{
    SetUnhandledExceptionFilter(myUnhandledExceptionFilter);
}


namespace OutputDebug{
Thread* threadInstance = nullptr;
const char* CRLF = "\r\n";
char time_buff[32];
const int MaxLineCount = 600*1000;

void formatTime(const SYSTEMTIME& time)
{
    _snprintf_s(time_buff, sizeof(time_buff), "%u%02u%02u %02u:%02u:%02u.%03u ",
                time.wYear,time.wMonth,
                time.wDay,time.wHour,time.wMinute,
                time.wSecond,time.wMilliseconds);
}

Thread::Thread(QObject *parent) :
    QThread(parent),
    m_lineCount(0)
{   
}

void Thread::run()
{
    m_mutex.lock();
    m_cond.wakeOne();
    m_mutex.unlock();
    std::unique_ptr<QFile> f;
    while(true)
    {
        m_mutex.lock();
        while(m_input.isEmpty())
        {
            m_cond.wait(&m_mutex);
        }
        m_input.swap(m_output);
        m_mutex.unlock();

        if (m_lineCount==0)
        {
            QString logFileName = generateFileName();
            f.reset(new QFile(logFileName));
            f->open(QFile::Append);
        }
        QTextStream stream(f.get());
        foreach(DebugMessgage* msg, m_output)
        {
            m_lineCount++;
            //日志消息格式 日期 时间 线程 级别 正文
            formatTime(msg->time);
            stream<<time_buff<<msg->threadId<<" "<<msg->level<<msg->msg<<CRLF;
        }
        qDeleteAll(m_output);
        m_output.clear();
        if (m_lineCount>=MaxLineCount)
        {
            m_lineCount = 0;
        }
    }
}

void Thread::installRun(bool isOutputConsole, bool isOutputFile)
{
    m_isOutputConsole = isOutputConsole;
    m_isOutputFile = isOutputFile;
    qInstallMsgHandler(our_message_handler);
    m_mutex.lock();
    start();
    m_cond.wait(&m_mutex);
    m_mutex.unlock();
    threadInstance = this;
}

QString Thread::generateFileName()
{
    QString hostName = getHostName();
    QString processName = getModuleName();

    QFileInfo f(processName);
    QString baseName  =  f.completeBaseName();
    QString path = f.path();

    DWORD pid = GetCurrentProcessId();
    QString currentTime = formatShortTime();

    QString fileName;
    QTextStream(&fileName)<<path<<"/"<<baseName<<"."<<pid<<"."
                         <<currentTime<<"."<<hostName<<".log";
    return fileName;
}

void Thread::queue(DebugMessgage* msg)
{
    m_mutex.lock();
    m_input.push_back(msg);
    m_cond.wakeOne();
    m_mutex.unlock();
}

void Thread::our_message_handler(QtMsgType type, const char * s)
{
    if (threadInstance)
    {
        if (threadInstance->m_isOutputConsole)
        {
            OutputDebugStringA(s);
            OutputDebugStringA(CRLF);
        }
        if (threadInstance->m_isOutputFile)
        {
            DebugMessgage* msg = new DebugMessgage;
            msg->msg = QString::fromLocal8Bit(s);

            GetLocalTime(&msg->time);
            msg->threadId = GetCurrentThreadId();
            switch(type)
            {
            case QtDebugMsg:
                msg->level = "";
                break;
            case QtWarningMsg:
                msg->level = "[Warning]";
                break;
            case QtCriticalMsg:
                msg->level = "[Critical]";
                break;
            case QtFatalMsg:
                msg->level = "[Fatal]";
                break;
            }
            threadInstance->queue(msg);
        }
    }
}
}//namespace OutputDebug

void installMsgHandler(bool isOutputConsole, bool isOutputFile)
{
    if (!OutputDebug::threadInstance)
    {
        OutputDebug::Thread *thread = new OutputDebug::Thread;
        thread->installRun(isOutputConsole, isOutputFile);
    }
}

void installMsgHandlerByFile()
{
    QString processName = getModuleName();
    QFileInfo f(processName);
    QString path = f.path();
    bool isOutputConsole = QFile::exists(path + "/" + "dbgview");
    bool isOutputFile = QFile::exists(path + "/" + "debug");
    installMsgHandler(isOutputConsole, isOutputFile);
}
}//namespace Utility
