/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/29

****************************************************************************/

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QDebug>
#include <unordered_map>
#include <queue>
#include "libuvpp.h"

namespace uv
{

class TcpServerLoop:public Loop
{
public:
    explicit TcpServerLoop():
        Loop()
    {
        m_async.reset(new Async(this, std::bind(&TcpServerLoop::onFunctor, this)));
    }

    void queueInLoop(const Callback &functor)
    {
        MutexLocker lock(&m_mutex);
        m_functors.push(functor);
        m_async->send();
    }

private:
    std::queue<Callback> m_functors;
    std::unique_ptr<Async> m_async;
    Mutex m_mutex;
    void onFunctor()
    {
        while (true)
        {
            Callback fun = nullptr;
            {
                MutexLocker lock(&m_mutex);
                if (!m_functors.empty())
                {
                    fun.swap(m_functors.front());
                    m_functors.pop();
                }
            }

            if (fun)
            {
                fun();
            }
            else{
                break;
            }
        }
    }
    DISABLE_COPY(TcpServerLoop)
};

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::weak_ptr<Connection> ConnectionWeakPtr;
class Connection:public std::enable_shared_from_this<Connection>
{
public:
    typedef std::function<void(const ConnectionPtr &)> Callback;
    typedef std::function<void(const ConnectionPtr &, const QByteArray &)> MessageCallback;

    explicit Connection(Loop *loop):
        m_loop(loop),
        m_tcp(loop)
    {
        static int id = 0;
        id++;
        m_id = id;
        qDebug()<<"Connection"<<m_id;

        m_tcp.onClose(std::bind(&Connection::handleClose, this));

    }

    ~Connection()
    {
        qDebug()<<"~Connection"<<m_id;
    }

    int id()
    {
        return m_id;
    }

    void shutdown()
    {
        m_tcp.shutdown();
    }

    void forceClose()
    {
        handleClose();
    }

    void write(const QByteArray &d)
    {
        m_tcp.write(d, nullptr);
    }

    Loop* loop()
    {
        return m_loop;
    }

    MessageCallback messageCallback;
    std::shared_ptr<void> context;

private:
    void connectionEstablished()
    {
        m_tcp.read_start(std::bind(&Connection::handleRead, this, std::placeholders::_1));
    }

    void connectionDestroyed()
    {                
        qDebug()<<"connectionDestroyed";
    }

    Callback closeCallback;
    friend class TcpServer;

private:
    Loop* m_loop;
    Tcp m_tcp;
    int m_id;
    void handleClose()
    {
        //qDebug()<<"handleClose";
        messageCallback = nullptr;
        context = nullptr;
        ConnectionPtr guardThis(shared_from_this());        
        closeCallback(guardThis);
    }

    void handleRead(const QByteArray &data)
    {
        //qDebug()<<"handleRead:"<<data;
        if (messageCallback)
        {
            messageCallback(shared_from_this(), data);
        }
    }
    DISABLE_COPY(Connection)
};

typedef std::function<void(const ConnectionPtr &)> ConnectionCallback;



class TcpServer
{
public:
    explicit TcpServer(TcpServerLoop* loop):
        m_loop(loop),
        m_tcpServer(loop)
    {

    }
    bool listen(const char *ip, int port)
    {
        m_tcpServer.bind(ip, port);
        return m_tcpServer.listen(std::bind(&TcpServer::onNewConnection, this))>=0;
    }

    ConnectionCallback connectionCallback;
    ConnectionCallback connectionCloseCallback;
    Connection::MessageCallback messageCallback;
private:
    TcpServerLoop* m_loop;
    Tcp m_tcpServer;
    std::unordered_map<int, ConnectionPtr> m_connections;

    void onNewConnection()
    {
        ConnectionPtr connection(new Connection(m_loop));
        m_tcpServer.accept(&connection->m_tcp);
        connection->connectionEstablished();

        m_connections.insert(std::make_pair(connection->id(), connection));
        connection->closeCallback = std::bind(&TcpServer::onConnectionClose, this, std::placeholders::_1);
        connection->messageCallback = messageCallback;
        //qDebug()<<"onNewConnection";
        if (connectionCallback)
        {
            connectionCallback(connection);
        }
    }

    void onConnectionClose(const ConnectionPtr &connection)
    {
        m_loop->queueInLoop(std::bind(&Connection::connectionDestroyed, connection));
        m_connections.erase(connection->id());
        //qDebug()<<"onConnectionClose";
        if (connectionCloseCallback)
        {
            connectionCloseCallback(connection);
        }
    }
    DISABLE_COPY(TcpServer)
};
}
#endif // TCPSERVER_H
