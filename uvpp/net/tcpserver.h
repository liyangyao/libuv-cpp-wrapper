/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/18

****************************************************************************/

#ifndef UVPP_TCPSERVER_H
#define UVPP_TCPSERVER_H

#include <uvpp/uvpp_tcp.h>

namespace uv{


class TcpConnection
{
public:
    TcpConnection(Loop* loop):
        m_loop(loop),
        m_tcp(loop)
    {

    }

    ~TcpConnection()
    {

    }

    void setContext(const std::shared_ptr<void> &context)
    {
        m_context = context;
    }

    Tcp* tcp()
    {
        return &m_tcp;
    }

    Loop* loop()
    {
        return m_loop;
    }

private:
    std::shared_ptr<void> m_context;
    Tcp m_tcp;
    Loop* m_loop;
};
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;


class TcpServer
{
public:
    typedef std::function<void(const TcpConnectionPtr &)> CallbackNewConnection;
    explicit TcpServer(Loop* loop):
        m_loop(loop),
        m_server(loop)
    {

    }

    bool listen(const char *ip, int port)
    {
        m_server.bind(ip, port);
        m_server.onConnection(std::bind(&TcpServer::handleNewConnection, this));
        return m_server.listen();
    }

    CallbackNewConnection onConnection;
private:
    Loop* m_loop;
    Tcp m_server;

    void handleNewConnection()
    {
        TcpConnectionPtr conn(new TcpConnection(m_loop));
        m_server.accept(conn->tcp());
        conn->tcp()->read_start();

        if (onConnection)
        {
            onConnection(conn);
        }

        conn->tcp()->onClose([conn]
        {
            conn->setContext(nullptr);
            conn->tcp()->onClose(nullptr);
        });
    }
    DISABLE_COPY(TcpServer)
};
}

#endif // UVPP_TCPSERVER_H
