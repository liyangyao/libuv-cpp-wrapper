#include "tcpconnection.h"
#include "uv.h"
#include <QDebug>

void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    static char BUFFER[65535];
    buf->base = BUFFER;
    buf->len = 65535;
}

void TcpConnection::read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    qDebug()<<"TcpConnection::read_cb nread="<<nread;
    TcpConnection *_this = (TcpConnection *)stream->data;
    if (nread<0)
    {
        qDebug()<<"Close";
        _this->handleClose();
    }
    else if (nread>0)
    {
        QByteArray data(buf->base, buf->len);
        if (_this->m_messageCallback)
        {
            _this->m_messageCallback(_this->shared_from_this(), data);
        }

//        qDebug()<<"call m_messageCallback:"<<data;
//        uv_write_t* req = new uv_write_t;
//        uv_write(req, stream, buf, 1, nullptr);

    }
}

TcpConnection::TcpConnection(net::Socket *socket):
    m_socket(socket)
{
    qDebug()<<"TcpConnection::TcpConnection";
    uv_read_start((uv_stream_t*)m_socket->handle(), alloc_cb, read_cb);
    m_socket->handle()->data = this;
}

TcpConnection::~TcpConnection()
{
    qDebug()<<"TcpConnection::~TcpConnection";
}

void TcpConnection::connectEstablished()
{
    if (m_connectionCallback)
    {
        m_connectionCallback(shared_from_this());
    }
}

void TcpConnection::connectDestroyed()
{

}

class WriteReqData
{
public:
    WriteReqData(const TcpConnectionPtr &c, const QByteArray &data):
        conn(c),
        content(data)
    {
        req.data = this;
        buf.base = content.data();
        buf.len = content.length();
    }

    uv_write_t req;
    QByteArray content;
    uv_buf_t buf;
    TcpConnectionWeakPtr conn;
};

void TcpConnection::write_cb(uv_write_t *req, int status)
{
    qDebug()<<"write_cb  status="<<status;
    if (status==0)
    {
        //"success"
    }
    WriteReqData *wrd = (WriteReqData *)req->data;
    TcpConnectionPtr p = wrd->conn.lock();
    if (p && p->m_writeCompleteCallback)
    {
        p->m_writeCompleteCallback(p);
    }
    delete wrd;
}

void TcpConnection::send(const QByteArray &data)
{
    WriteReqData *wrd = new WriteReqData(shared_from_this(), data);
    uv_write(&wrd->req, (uv_stream_t*)m_socket->handle(), &wrd->buf, 1, write_cb);
}

void afterShutdown(uv_shutdown_t* req, int status)
{
    delete req;
}

void TcpConnection::shutdown()
{
    uv_shutdown_t *req = new uv_shutdown_t;
    uv_shutdown(req, (uv_stream_t*)m_socket->handle(), afterShutdown);
}

void TcpConnection::handleClose()
{
    TcpConnectionPtr guardThis(shared_from_this());
//    if (m_connectionCallback)
//    {
//        m_connectionCallback(guardThis);
//    }

    // must be the last line
    if (m_closeCallback)
    {
        m_closeCallback(guardThis);
    }


}
