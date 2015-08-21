#include <algorithm>
#include <time.h>
#include <array>
#include "mainform.h"
#include <QApplication>
#include <QTextCodec>
#include <QDebug>
#include <QDateTime>
#include <uvpp/uvpp.h>
#include <botan/botan.h>
#include <DbgHelp.h>
#include <Windows.h>
#include "botan_wrapper.h"
#include <misc/tcpserver.h>

#pragma execution_character_set("utf-8")

#pragma comment(lib, "Dbghelp.lib")

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
}

class Buffer
{
public:
    explicit Buffer(QByteArray *data):
        m_data(data),
        m_pos(0)
    {

    }

    int remain()
    {
        return m_data->size() - m_pos;
    }

    QByteArray read(int n)
    {
        int pos = m_pos + n;
        if (pos>m_data->size())
        {
            pos = m_data->size();
        }
        QByteArray tmp = QByteArray::fromRawData(m_data->constData()+m_pos, pos-m_pos);
        m_pos = pos;
        return tmp;
    }

private:
    QByteArray *m_data;
    int m_pos;
};

int gSessionCount = 0;
class Session
{
public:    
    Session(const uvpp::ConnectionPtr &conn, const QByteArray& data):
        m_local(conn),
        m_tcp(conn->loop()),
        m_remoteConnected(false)
    {
        gSessionCount++;
        qDebug()<<"Session Constructor("<<this<<")"<<"("<<m_local.get()<<")";
        localMessage(data);
        conn->messageCallback = std::bind(&Session::localMessage, this, std::placeholders::_2);
        m_tcp.connect("45.62.109.185", 443, std::bind(&Session::remoteConnected, this, conn, std::placeholders::_1));
        m_tcp.onRead(std::bind(&Session::remoteMessage, this, std::placeholders::_1));
        m_tcp.onClose(std::bind(&Session::onRemoteClosed, this));
    }

    ~Session()
    {
        gSessionCount--;
        qDebug()<<"Session Destructor("<<this<<")--->remain="<<gSessionCount<<"("<<m_local.get()<<")";
    }

private:
    uvpp::Tcp m_tcp;
    QByteArray m_dataToWrite;
    uvpp::ConnectionPtr m_local;
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

    void remoteConnected(const uvpp::ConnectionWeakPtr &conn, bool connected)
    {
        //为什么这儿要加入conn呢, 因为要监控远程是否已关闭,这关系着 session是否还活着
        uvpp::ConnectionPtr p = conn.lock();
        if (!p || !p->context)
        {
            qDebug()<<"--------------------------remoteConnected expired----------------------------";
            return;
        }
        if (connected)
        {
            qDebug()<<"Session remote connected("<<this<<")"<<"("<<m_local.get()<<")";
            m_remoteConnected = true;
            if (!m_dataToWrite.isEmpty())
            {
                m_tcp.write(m_dataToWrite, nullptr);
                m_dataToWrite.clear();
            }
            m_tcp.read_start();
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
        qDebug()<<"Session onRemoteClosed("<<this<<")"<<"("<<m_local.get()<<")";
        m_local->shutdown();
    }
};

class AuthSession:public std::enable_shared_from_this<AuthSession>
{
public:
    AuthSession(const uvpp::ConnectionPtr &conn):
        m_status(0),
        m_urlLen(0),
        m_local(conn)
    {
        qDebug()<<"AuthSession Destructor("<<this<<")("<<m_local.get()<<")";
        m_buffer.reset(new Buffer(&m_recved));
        conn->messageCallback = std::bind(&AuthSession::localMessage, this, std::placeholders::_1, std::placeholders::_2);
    }

    ~AuthSession()
    {
        qDebug()<<"AuthSession Destructor("<<this<<")("<<m_local.get()<<")";
    }

private:
    int m_status;
    QByteArray m_recved;
    std::unique_ptr<Buffer> m_buffer;
    int m_addrType;
    int m_urlLen;
    uvpp::ConnectionPtr m_local;
    void localMessage(const uvpp::ConnectionPtr &conn, const QByteArray &data)
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
            static QByteArray respon = QByteArray::fromHex("0500");
            conn->write(respon);
        }
        else if (m_status==1)
        {
            //05 01 00 03 0e 7777772e676f6f676c652e636f6d 01bb
            m_recved.append(data);
            if (m_urlLen==0)
            {
                if (m_buffer->remain()<5)
                {
                    return;
                }
                QByteArray d = m_buffer->read(5);
                m_addrType = d[3];
                m_urlLen = d[4];
                //qDebug()<<"m_addrType="<<m_addrType<<"m_urlLen="<<m_urlLen;
            }
            if (m_buffer->remain()<m_urlLen+2)
            {
                return;
            }
            std::shared_ptr<AuthSession> guard(shared_from_this());
            QString aurl = m_buffer->read(m_urlLen);


            m_status = 2;

            static const char res [] = { 5, 0, 0, 1, 0, 0, 0, 0, 16, 16 };
            static const QByteArray response(res, 10);
            conn->write(response);

            std::shared_ptr<Session> session(new Session(conn, m_recved.right(m_recved.length()-3)));
            conn->context = session;
            qDebug()<<"Session("<<session.get()<<")--->"<<aurl;
        }
    }
};


void runThread()
{
    uvpp::Thread* thread = new uvpp::Thread([&thread]()
    {
        uvpp::Loop loop;
        uvpp::TcpServer server(&loop);
        server.onNewConnection = [](const uvpp::ConnectionPtr &conn)
        {
            std::shared_ptr<AuthSession> authSession(new AuthSession(conn));
            conn->context = authSession;
        };

        qDebug()<<"listen:"<< server.listen("0.0.0.0", 1081);
        loop.run();
        delete thread;
    });
    Q_UNUSED(thread)


}

void test()
{
    uvpp::Thread* thread = new uvpp::Thread([&thread]()
    {
        uvpp::Loop loop;

        {
        uvpp::Tcp tcp(&loop);
        tcp.connect("111.13.100.91", 80, [](bool connected)
        {
            qDebug()<<"Connected www.baidu.com";
        });
        }
        uvpp::Timer timer(&loop);
        timer.start([]()
        {

        }, 1000, 1000);
        loop.run();

    });
    Q_UNUSED(thread)
}

int main(int argc, char *argv[])
{
    InstallDump();    
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QApplication a(argc, argv);
    qDebug()<<uv_version_string();
    QFont f = a.font();
    f.setPointSize(9);
    a.setFont(f);

    MainForm w;
    w.show();
    runThread();
    //test();


    return a.exec();
}
