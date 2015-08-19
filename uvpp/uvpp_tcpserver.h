/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/18

****************************************************************************/

#ifndef UVPP_TCPSERVER_H
#define UVPP_TCPSERVER_H

#include <vector>
#include <unordered_map>
#include <uvpp_idle.h>
#include <uvpp_connection.h>


namespace uvpp{

class TcpServer
{
public:
    explicit TcpServer(Loop* loop):
        m_loop(loop),
        m_tcpServer(loop),
        m_idle(loop, std::bind(&TcpServer::handleCallFunctor, this))
    {

    }

    bool listen(const char *ip, int port)
    {
        m_tcpServer.bind(ip, port);
        m_tcpServer.onNewConnection(std::bind(&TcpServer::handleNewConnection, this));
        return m_tcpServer.listen();
    }

    Connection::NewConnectionCallback onNewConnection;
    Connection::CloseCallback onConnectionClose;
    Connection::MessageCallback onMessage;
private:
    Loop* m_loop;
    Tcp m_tcpServer;
    Idle m_idle;
    std::vector<Functor> m_functors;

    std::unordered_map<int, ConnectionPtr> m_connections;

    void handleNewConnection()
    {
        std::unique_ptr<Tcp> tcp(new Tcp(m_loop));
        m_tcpServer.accept(tcp.get());
        ConnectionPtr connection = std::make_shared<Connection>(tcp);
        connection->closeCallback = std::bind(&TcpServer::hanleConnectionClose, this, std::placeholders::_1);
        connection->messageCallback = onMessage;
        connection->connectionEstablished();
        m_connections.insert(std::make_pair(connection->id(), connection));
        if (onNewConnection)
        {
            onNewConnection(connection);
        }
    }

    void hanleConnectionClose(const ConnectionPtr &connection)
    {
        m_functors.emplace_back(std::bind(&Connection::connectionDestroyed, connection));
        m_idle.start();
        m_connections.erase(connection->id());
        if (onConnectionClose)
        {
            onConnectionClose(connection);
        }
    }

    void handleCallFunctor()
    {
        std::for_each(m_functors.begin(), m_functors.end(), [](const Functor &fun)
        {
           fun();
        });
        m_functors.clear();
        m_idle.stop();
    }

    DISABLE_COPY(TcpServer)
};
}

#endif // UVPP_TCPSERVER_H
