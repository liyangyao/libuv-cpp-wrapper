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
#include <uvpp/net/tcpserver.h>
#include <QDebug>
#include "li_utility.h"

#pragma execution_character_set("utf-8")

#pragma comment(lib, "Dbghelp.lib")

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
class Session:public std::enable_shared_from_this<Session>
{
public:
    Session(const uv::TcpConnectionPtr &conn, const QByteArray& data):
        m_local(conn),
        m_tcp(conn->loop()),
        m_remoteConnected(false)
    {
        gSessionCount++;
        qDebug()<<"Session Constructor("<<this<<")"<<"("<<m_local.get()<<")";
        localMessage(data.constData(), data.size());
        conn->tcp()->onData(std::bind(&Session::localMessage, this, std::placeholders::_1, std::placeholders::_2));
    }

    void init()
    {
        m_tcp.connect("45.62.109.185", 443, std::bind(&Session::remoteConnected, this, shared_from_this(), std::placeholders::_1));
        m_tcp.onData(std::bind(&Session::remoteMessage, this, std::placeholders::_1, std::placeholders::_2));
        m_tcp.onEnd(std::bind(&Session::onRemoteClosed, this));
    }

    ~Session()
    {
        gSessionCount--;
        qDebug()<<"Session Destructor("<<this<<")--->remain="<<gSessionCount<<"("<<m_local.get()<<")";
    }

private:
    uv::Tcp m_tcp;
    QByteArray m_dataToWrite;
    uv::TcpConnectionPtr m_local;
    Botan::Encryptor m_encryptor;
    bool m_remoteConnected;

    void localMessage(const char* d, int size)
    {
        QByteArray data = QByteArray::fromRawData(d, size);
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

    void remoteConnected(const std::weak_ptr<Session> &ref, int status)
    {

        if (ref.expired())
        {
            qDebug()<<"--------------------------Session expired----------------------------";
            return;
        }
        if (status==0)
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

    void remoteMessage(const char *data, int size)
    {
        QByteArray ba = QByteArray::fromRawData(data, size);
        QByteArray output = m_encryptor.decrypt(ba);
        //qDebug()<<"remoteMessage size="<<data.size()<<" outputsize="<<output.size();
        //qDebug()<<output;
        m_local->tcp()->write(output);

    }

    void onRemoteClosed()
    {
        qDebug()<<"Session onRemoteClosed("<<this<<")"<<"("<<m_local.get()<<")";
        m_local->tcp()->shutdown();
    }
};
typedef std::shared_ptr<Session> SessionPtr;
Q_DECLARE_METATYPE(SessionPtr);

class AuthSession:public std::enable_shared_from_this<AuthSession>
{
public:
    AuthSession(const uv::TcpConnectionPtr &conn):
        m_status(0),
        m_urlLen(0),
        m_local(conn)
    {
        qDebug()<<"AuthSession create("<<this<<")("<<m_local.get()<<")";
        m_buffer.reset(new Buffer(&m_recved));
        conn->tcp()->onData(std::bind(&AuthSession::localMessage, this, std::placeholders::_1, std::placeholders::_2));
    }

    ~AuthSession()
    {
        qDebug()<<"AuthSession destroy("<<this<<")("<<m_local.get()<<")";
    }

private:
    int m_status;
    QByteArray m_recved;
    std::unique_ptr<Buffer> m_buffer;
    int m_addrType;
    int m_urlLen;
    uv::TcpConnectionPtr m_local;

    void localMessage(const char *d, int size)
    {
        QByteArray data = QByteArray::fromRawData(d, size);
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
            m_local->tcp()->write(respon);
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
            m_status = 2;




//std::shared_ptr<AuthSession> guard(shared_from_this());



            static const char res [] = { 5, 0, 0, 1, 0, 0, 0, 0, 16, 16 };
            static const QByteArray response(res, 10);
            m_local->tcp()->write(response);

            QString url = QString::fromUtf8(m_buffer->read(m_urlLen));

            SessionPtr session(new Session(m_local, m_recved.right(m_recved.length()-3)));
            session->init();
            qDebug()<<"switch";
            m_local->setContext(session);

            qDebug()<<"Session("<<session.get()<<")--->"<<url;





        }
    }
};
typedef std::shared_ptr<AuthSession> AuthSessionPtr;
Q_DECLARE_METATYPE(AuthSessionPtr);


void runThread()
{
    uv::Thread* thread = new uv::Thread([&thread]()
    {
        uv::Loop loop;
        uv::TcpServer server(&loop);
        server.onConnection = [&loop](const uv::TcpConnectionPtr &conn)
        {
           AuthSessionPtr authSession(new AuthSession(conn));
           conn->setContext(authSession);
        };

        qDebug()<<"listen:"<< server.listen("0.0.0.0", 1081);
        loop.run();
        delete thread;
    });
    Q_UNUSED(thread)


}

void test()
{
    uv::Thread* thread = new uv::Thread([&thread]()
    {
        uv::Loop loop;


            uv::Tcp tcp(&loop);
            uv::Timer timer(&loop);
            tcp.connect("111.13.100.91", 80, [&](int status)
            {
                qDebug()<<"Connected www.baidu.com status="<<status;


                timer.start([&]()
                {
                    qDebug()<<"begin Call shutdown";
                    tcp.shutdown([]
                    {
                        qDebug()<<"shutdown now";
                    });
                }, 1000, 0);
            });
            tcp.onEnd([]()
            {
                qDebug()<<"onEnd";
            });


        loop.run();

    });
    Q_UNUSED(thread)
}

int main(int argc, char *argv[])
{
    Utility::installMsgHandler(true, true);
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
