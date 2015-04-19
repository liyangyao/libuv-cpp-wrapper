#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H


#include <QObject>
#include <memory>
#include "Callbacks.h"
#include "socket.h"


class TcpConnection:public std::enable_shared_from_this<TcpConnection>
{
public:
    explicit TcpConnection(net::Socket *socket);
    ~TcpConnection();
    void connectEstablished();
    void connectDestroyed();
    void send(const QByteArray &data);
    void shutdown();
    void setConnectionCallback(const ConnectionCallback &cb) { m_connectionCallback = cb; }
    void setCloseCallback(const CloseCallback &cb) { m_closeCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { m_writeCompleteCallback = cb; }

private:
    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    CloseCallback m_closeCallback;
    WriteCompleteCallback m_writeCompleteCallback;
    net::Socket* m_socket;
    void handleClose();
    static void read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
    static void write_cb(uv_write_t *req, int status);
};

#endif // TCPCONNECTION_H
