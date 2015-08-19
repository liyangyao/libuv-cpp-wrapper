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
        m_tcpServer(loop)
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

        qDebug()<<"hanleConnectionClose #1";
        if (onConnectionClose)
        {
            onConnectionClose(connection);
        }
        connection->connectionDestroyed();
        m_connections.erase(connection->id());
        qDebug()<<"hanleConnectionClose #2";
    }
    DISABLE_COPY(TcpServer)
};
}

#endif // UVPP_TCPSERVER_H
