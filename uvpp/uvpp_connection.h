/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/18

****************************************************************************/

#ifndef UVPP_CONNECTION_H
#define UVPP_CONNECTION_H

#include <uvpp_tcp.h>

namespace uvpp{

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::weak_ptr<Connection> ConnectionWeakPtr;
class Connection:public std::enable_shared_from_this<Connection>
{
public:
    typedef std::function<void(const ConnectionPtr &,const QByteArray &)> MessageCallback;
    typedef std::function<void(const ConnectionPtr &)> NewConnectionCallback;
    typedef std::function<void(const ConnectionPtr &)> CloseCallback;
    MessageCallback messageCallback;
    CloseCallback closeCallback;
    std::shared_ptr<void> context;
    explicit Connection(std::unique_ptr<Tcp> &tcp):
        m_tcp(std::move(tcp))
    {
        qDebug()<<"Connection Constructor("<<this<<")";
        static int id = 0;
        id++;
        m_id = id;
    }

    ~Connection()
    {        
        qDebug()<<"Connection Destructor("<<this<<")";
    }

    int id()
    {
        return m_id;
    }

    void shutdown()
    {
        if (m_tcp)
        {
            m_tcp->shutdown();
        }
    }

    void write(const QByteArray &d)
    {
        if (m_tcp)
        {
            m_tcp->write(d);
        }
    }

    Loop* loop()
    {
        if (m_tcp)
        {
            return (Loop *)m_tcp->handle()->loop->data;
        }
        return nullptr;
    }

    void connectionEstablished()
    {
        if (m_tcp)
        {
            m_tcp->onRead(std::bind(&Connection::handleRead, this, std::placeholders::_1));
            m_tcp->onDisconnect(std::bind(&Connection::handleClose, this));
            m_tcp->read_start();
        }
    }

    void connectionDestroyed()
    {
        qDebug()<<"Connection connectionDestroyed("<<this<<")";
        messageCallback = nullptr;
        closeCallback = nullptr;
        context = nullptr;
        m_tcp.reset();
    }

private:
    DISABLE_COPY(Connection)
    void handleClose()
    {
        //qDebug()<<"Connection handleClose("<<this<<")";
        if (closeCallback)
        {
            closeCallback(shared_from_this());
        }
    }

    void handleRead(const QByteArray &data)
    {
        //qDebug()<<"Connection handleRead("<<this<<")";
        if (messageCallback)
        {
            messageCallback(shared_from_this(), data);
        }
    }

    std::unique_ptr<Tcp> m_tcp;
    int m_id;
};
}

#endif // UVPP_CONNECTION_H
