#include <algorithm>
#include "mainform.h"
#include <QApplication>

#include "libuvpp.h"
#include "tcpserver.h"
#include <botan/botan.h>
#include <array>
#include <QDateTime>
#include <DbgHelp.h>
#include <time.h>
#include "botan_wrapper.h"

#pragma execution_character_set("utf-8")

#pragma comment(lib, "Dbghelp.lib")

void DisableSetUnhandledExceptionFilter()
{
    void *addr = (void*)GetProcAddress(LoadLibrary(L"kernel32.dll"), "SetUnhandledExceptionFilter");
    if (addr)
    {
        unsigned char code[16];
        int size = 0;
        code[size++] = 0x33;
        code[size++] = 0xC0;
        code[size++] = 0xC2;
        code[size++] = 0x04;
        code[size++] = 0x00;

        DWORD dwOldFlag, dwTempFlag;
        VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
        WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
        VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
    }
}

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
    wchar_t szProgramPath[MAX_PATH] = {0};
    if(GetModuleFileName(NULL, szProgramPath, MAX_PATH))
    {
           LPTSTR lpSlash = wcsrchr(szProgramPath, '\\');
           if(lpSlash)
           {
               *(lpSlash + 1) = '\0';
           }
    }
    wchar_t szDumpFile[MAX_PATH] = {0};
    swprintf_s(szDumpFile, MAX_PATH, L"%s%d.dmp", szProgramPath, time(NULL));

    HANDLE lhDumpFile = CreateFile(szDumpFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
    MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
    loExceptionInfo.ExceptionPointers = ExceptionInfo;
    loExceptionInfo.ThreadId = GetCurrentThreadId();
    loExceptionInfo.ClientPointers = true;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile, MiniDumpWithIndirectlyReferencedMemory, &loExceptionInfo, NULL, NULL);
    CloseHandle(lhDumpFile);

    return EXCEPTION_EXECUTE_HANDLER;

}

void InstallDump()
{
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    DisableSetUnhandledExceptionFilter();
}

int gSessionCount = 0;
class Session
{
public:
    Session(const uv::ConnectionPtr &conn, const QByteArray& data):
        m_local(conn),
        m_tcp(conn->loop()),
        m_remoteConnected(false)
    {
        gSessionCount++;
        qDebug()<<"Session Constructor("<<this<<")"<<gSessionCount;
        localMessage(data);
        conn->messageCallback = std::bind(&Session::localMessage, this, std::placeholders::_2);
        m_tcp.connect("45.62.109.185", 443);
        m_tcp.onConnect(std::bind(&Session::remoteConnected, this, std::placeholders::_1));
        m_tcp.onClose(std::bind(&Session::onRemoteClosed, this));
    }

    ~Session()
    {
        gSessionCount--;
        qDebug()<<"Session Destructor("<<this<<")"<<gSessionCount;
    }

private:
    uv::Tcp m_tcp;
    QByteArray m_dataToWrite;
    uv::ConnectionPtr m_local;
    Botan::Encryptor m_encryptor;
    bool m_remoteConnected;

    void localMessage(const QByteArray &data)
    {
        if (data.isEmpty()) return;
        //qDebug()<<"localMessage size="<<data.length();
        QByteArray output = m_encryptor.encrypt(data);
        if (m_remoteConnected)
        {
            //qDebug()<<"WRITE TO REMOTE";
            m_tcp.write(output, nullptr);
        }
        else{
            m_dataToWrite.append(output);
        }
    }

    void remoteConnected(bool connected)
    {
        if (connected)
        {
            qDebug()<<"Session remote connected("<<this<<")";
            m_remoteConnected = true;
            if (!m_dataToWrite.isEmpty())
            {
                m_tcp.write(m_dataToWrite, nullptr);
                m_dataToWrite.clear();
            }
            m_tcp.onMessage(std::bind(&Session::remoteMessage, this, std::placeholders::_1));
        }
    }

    void remoteMessage(const QByteArray &data)
    {
        QByteArray output = m_encryptor.decrypt(data);
        //qDebug()<<"remoteMessage size="<<data.size()<<" outputsize="<<output.size();
        //qDebug()<<output;
        m_local->write(output);

    }

    void onRemoteClosed()
    {
        qDebug()<<"Session onRemoteClosed("<<this<<")";
        m_local->shutdown();
    }
};

class AuthSession
{
public:
    AuthSession(const uv::ConnectionPtr &conn):
        m_status(0)
    {
        conn->messageCallback = std::bind(&AuthSession::localMessage, this, std::placeholders::_1, std::placeholders::_2);
    }

    ~AuthSession()
    {
    }

private:
    int m_status;
    QByteArray m_recved;
    void localMessage(const uv::ConnectionPtr &conn, const QByteArray &data)
    {
        if (m_status==0)
        {
            QString hex = data.toHex();
            //qDebug()<<"recv:"<< hex;
            m_status = 1;
            //REQUEST:
            //client连接proxy的第一个报文信息，进行认证机制协商
            //一般是 hex: 05 01 00 即：版本5，1种认证方式，NO AUTHENTICATION REQUIRED(无需认证 0x00)
            //RESPON:
            //proxy从METHODS字段中选中一个字节(一种认证机制)，并向Client发送响应报文:
            //一般是 hex: 05 00 即：版本5，无需认证
            QByteArray respon=QByteArray::fromHex("0500");
            conn->write(respon);
        }
        else if (m_status==1)
        {
            m_recved.append(data);
            if (m_recved.length()<=7)
            {
                return;
            }
            static const char res [] = { 5, 0, 0, 1, 0, 0, 0, 0, 16, 16 };
            static const QByteArray response(res, 10);
            conn->write(response);

            m_status = 2;
            //qDebug()<<m_recved.mid(4, m_recved.length()-6).toHex();
            QByteArray url = m_recved.mid(5, m_recved.length()-7);

            qDebug()<<"go to session:"<<url;
            std::shared_ptr<Session> session(new Session(conn, data.right(m_recved.length()-3)));
            conn->context = session;
        }
    }
};


void runThread()
{
    uv::Thread* thread = new uv::Thread;
    thread->create([]()
    {
        uv::Loop loop;
        uv::TcpServer server(&loop);
        server.connectionCallback = [](const uv::ConnectionPtr &conn)
        {
            std::shared_ptr<AuthSession> authSession(new AuthSession(conn));
            conn->context = authSession;
        };

        qDebug()<<"listen:"<< server.listen("0.0.0.0", 1081);
        loop.run();
    });

}

int newMain(int argc, char *argv[])
{
    InstallDump();
    QApplication a(argc, argv);

//    try
//    {
//        Botan::LibraryInitializer init;
//    }
//    catch(std::exception &e)
//    {
//        qDebug()<<e.what();
//    }

    MainForm w;
    w.show();
    runThread();
    Botan::Encryptor e;

//    e.setup("AES-128/CFB", "Njg1MjgxZD", 16, 16);
//    QByteArray d1 = e.encrypt("Hello World!");
//    QByteArray d2 = e.decrypt(d1);
//    qDebug()<<d1.toHex();
//    qDebug()<<d2;


    return a.exec();
}

int main(int argc, char *argv[])
{
    __try
    {
        newMain(argc, argv);
    }
    __except( MyUnhandledExceptionFilter(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER )
    {

    }

}
