#include "tcpserver.h"
#include "socket.h"
#include "tcpconnection.h"
#include <QDebug>

void TcpServer::connection_cb(uv_stream_t* server, int status)
{
    qDebug()<<"New Connection status="<<status;
    TcpServer *_this = (TcpServer *)server->data;
    if (status==0)
    {
        net::Socket *s = new net::Socket(_this->m_loop);
        uv_accept(server, (uv_stream_t* )s->handle());
        TcpConnectionPtr conn(new TcpConnection(s));
        conn->setCloseCallback(std::bind(&TcpServer::removeConnection, _this, std::placeholders::_1));
        conn->setMessageCallback(_this->m_messageCallback);
        conn->setConnectionCallback(_this->m_connectionCallback);
        conn->setWriteCompleteCallback(_this->m_writeCompleteCallback);
        _this->m_connections[(int)conn.get()] = conn;
        conn->connectEstablished();
    }
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    qDebug()<<"Call TcpServer::removeConnection";
    if (m_closeCallback)
    {
        m_closeCallback(conn);
    }
    m_connections.erase((int)conn.get());
}

TcpServer::TcpServer(EventLoop *loop):
    m_loop(loop)
{
    m_socket = std::unique_ptr<net::Socket>(new net::Socket(m_loop));
    m_socket->handle()->data = this;
}

bool TcpServer::listen(int port)
{
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", port, &addr);
    int err = uv_tcp_bind(m_socket->handle(), (const struct sockaddr*) &addr, 0);
    if (err==0)
    {
        err = uv_listen((uv_stream_t*)m_socket->handle(), 126, connection_cb);
    }
    if (err!=0)
    {
        //uv_strerror(err);
    }
    return err==0;
}
