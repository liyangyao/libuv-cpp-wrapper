#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <memory>
#include <map>
#include "eventloop.h"
#include "socket.h"
#include "tcpconnection.h"

class Socket;

class TcpServer
{
public:
    explicit TcpServer(EventLoop *loop);
    bool listen(int port);
    void setConnectionCallback(const ConnectionCallback &cb) { m_connectionCallback = cb; }
    void setCloseCallback(const CloseCallback &cb) { m_closeCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }

private:
    std::unique_ptr<net::Socket> m_socket;
    EventLoop *m_loop;

    typedef std::map<int, TcpConnectionPtr> ConnectionMap;
    ConnectionMap m_connections;
    ConnectionCallback m_connectionCallback;
    CloseCallback m_closeCallback;
    MessageCallback m_messageCallback;
    WriteCompleteCallback m_writeCompleteCallback;
    static void connection_cb(uv_stream_t* server, int status);
    void removeConnection(const TcpConnectionPtr& conn);
};

#endif // TCPSERVER_H
