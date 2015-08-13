/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/29

****************************************************************************/

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QDebug>
#include <unordered_map>
#include <deque>
#include "libuvpp.h"

namespace uv
{

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::weak_ptr<Connection> ConnectionWeakPtr;
typedef std::function<void(const ConnectionPtr &)> ConnectionCallback;
typedef std::function<void(const ConnectionPtr &)> ConnectionCloseCallback;
typedef std::function<void(const ConnectionPtr &, const QByteArray &)> ConnectionMessageCallback;

class AsyncFunctor
{
public:
    explicit AsyncFunctor(Loop* loop)
    {
        m_async.reset(new Async(loop, std::bind(&AsyncFunctor::onFunctor, this)));
    }

    void queue(const Callback &functor)
    {
        MutexLocker lock(&m_mutex);
        m_functors.push_back(functor);
        m_async->send();
    }

private:
    std::deque<Callback> m_functors;
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
                    m_functors.pop_front();
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
    DISABLE_COPY(AsyncFunctor)
};


class Connection:public std::enable_shared_from_this<Connection>
{
public:
    explicit Connection(Loop *loop):
        m_loop(loop),
        m_tcp(loop)
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

    ConnectionMessageCallback messageCallback;
    std::shared_ptr<void> context;

private:
    void connectionEstablished()
    {
        m_tcp.onClose(std::bind(&Connection::handleClose, this));
        m_tcp.read_start(std::bind(&Connection::handleRead, this, std::placeholders::_1));
    }

    void connectionDestroyed()
    {                
        qDebug()<<"Connection connectionDestroyed("<<this<<")";
        context = nullptr;
    }

    ConnectionCallback closeCallback;
    friend class TcpServer;

private:
    Loop* m_loop;
    Tcp m_tcp;
    int m_id;
    void handleClose()
    {
        qDebug()<<"Connection handleClose("<<this<<")";
        ConnectionPtr guardThis(shared_from_this());        
        closeCallback(guardThis);
    }

    void handleRead(const QByteArray &data)
    {
        //qDebug()<<"Connection handleRead("<<this<<")";
        if (messageCallback)
        {
            messageCallback(shared_from_this(), data);
        }
    }
    DISABLE_COPY(Connection)
};

class TcpServer
{
public:
    explicit TcpServer(Loop* loop):
        m_loop(loop),
        m_tcpServer(loop),
        m_asyncFunctor(loop)
    {

    }
    bool listen(const char *ip, int port)
    {
        m_tcpServer.bind(ip, port);
        return m_tcpServer.listen(std::bind(&TcpServer::handleConnection, this));
    }

    ConnectionCallback onConnection;
    ConnectionCloseCallback onConnectionClose;
    ConnectionMessageCallback onMessage;
private:
    Loop* m_loop;
    Tcp m_tcpServer;
    AsyncFunctor m_asyncFunctor;
    std::unordered_map<int, ConnectionPtr> m_connections;

    void handleConnection()
    {
        ConnectionPtr connection(new Connection(m_loop));
        m_tcpServer.accept(&connection->m_tcp);
        connection->connectionEstablished();

        m_connections.insert(std::make_pair(connection->id(), connection));
        connection->closeCallback = std::bind(&TcpServer::hanleConnectionClose, this, std::placeholders::_1);
        connection->messageCallback = onMessage;
        //qDebug()<<"onNewConnection";
        if (onConnection)
        {
            onConnection(connection);
        }
    }

    void hanleConnectionClose(const ConnectionPtr &connection)
    {
        m_asyncFunctor.queue(std::bind(&Connection::connectionDestroyed, connection));
        m_connections.erase(connection->id());
        if (onConnectionClose)
        {
            onConnectionClose(connection);
        }
    }
    DISABLE_COPY(TcpServer)
};
}
#endif // TCPSERVER_H
