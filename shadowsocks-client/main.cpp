#include <algorithm>
#include "mainform.h"
#include <QApplication>

#include "libuvpp.h"
#include "tcpserver.h"
#include <botan/botan.h>
#include <array>
#include "encryptor.h"

#pragma execution_character_set("utf-8")



class Session
{
public:
    Session(const uv::ConnectionPtr &conn, const QByteArray& data):
        m_local(conn),
        m_tcp(conn->loop()),
        m_remoteConnected(false)
    {
        qDebug()<<"Session Constructor("<<this<<")";
        localMessage(data);
        conn->messageCallback = std::bind(&Session::localMessage, this, std::placeholders::_2);
        m_tcp.connect("45.62.109.185", 443, std::bind(&Session::remoteConnected, this, std::placeholders::_1));
        m_tcp.setCloseCallback(std::bind(&Session::onRemoteClosed, this));
    }

    ~Session()
    {
        qDebug()<<"Session Destructor("<<this<<")";
    }

private:
    uv::Tcp m_tcp;
    QByteArray m_dataToWrite;
    uv::ConnectionWeakPtr m_local;
    Encryptor m_encryptor;
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
            qDebug()<<"WRITE TO LOCAL";
            m_dataToWrite.append(output);
        }
    }

    void remoteConnected(int status)
    {
        if (status==0)
        {
            qDebug()<<"connected";
            m_remoteConnected = true;
            if (!m_dataToWrite.isEmpty())
            {
                m_tcp.write(m_dataToWrite, nullptr);
                m_dataToWrite.clear();
                //qDebug()<<"WRITE DATATOWRITE";
            }
            m_tcp.read_start(std::bind(&Session::remoteMessage, this, std::placeholders::_1));
            //qDebug()<<"END remoteConnected";
        }
    }

    void remoteMessage(const QByteArray &data)
    {
        QByteArray output = m_encryptor.decrypt(data);
        //qDebug()<<"remoteMessage size="<<data.size()<<" outputsize="<<output.size();
        //qDebug()<<output;
        uv::ConnectionPtr conn = m_local.lock();
        if (conn)
        {
            conn->write(output);
        }
        else{
            qDebug()<<"Find.................!!!!!!!!!!!!!!!!!";
        }
    }

    void onRemoteClosed()
    {
        qDebug()<<"Session onRemoteClosed("<<this<<")";
        uv::ConnectionPtr conn = m_local.lock();
        if (conn)
        {
            conn->forceClose();
        }
    }
};

class AuthSession
{
public:
    AuthSession(const uv::ConnectionPtr &conn):
        m_status(0)
    {
        qDebug()<<"AuthSession Constructor("<<this<<")";
        conn->messageCallback = std::bind(&AuthSession::localMessage, this, std::placeholders::_1, std::placeholders::_2);
    }

    ~AuthSession()
    {
        qDebug()<<"AuthSession Destructor("<<this<<")";
    }

private:
    int m_status;
    QByteArray m_recved;
    void localMessage(const uv::ConnectionPtr &conn, const QByteArray &data)
    {
        if (m_status==0)
        {
            QString hex = data.toHex();
            qDebug()<<"recv:"<< hex;
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
            qDebug()<<m_recved.mid(4, m_recved.length()-6).toHex();
            QByteArray url = m_recved.mid(5, m_recved.length()-7);
            //int port = data.right(2).toInt();
            qDebug()<<url;



            qDebug()<<"go to session";
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
        uv::TcpServerLoop loop;
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



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try
    {
        Botan::LibraryInitializer init;
    }
    catch(std::exception &e)
    {
        qDebug()<<e.what();
    }

    MainForm w;
    w.show();
    runThread();
    qDebug()<< QByteArray::fromHex("7777772e676f6f676c652e636f6d");




    return a.exec();
}
