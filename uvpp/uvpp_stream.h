/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#ifndef UVPP_STREAM_H
#define UVPP_STREAM_H

#include "uvpp_define.h"
#include "uvpp_handle.h"
#include "uvpp_loop.h"
#include "libuv/queue.h"

namespace uvpp{

struct ReqStatus
{
    bool expired;
    QUEUE node;
};

template <typename HANDLE_T>
class Stream: public Handle<HANDLE_T>
{
public:
    explicit Stream():
        Handle<HANDLE_T>()
    {
        QUEUE_INIT(&m_reqStatusQueue);
    }

    ~Stream()
    {
        QUEUE* q;
        QUEUE_FOREACH(q, &m_reqStatusQueue)
        {
            ReqStatus* req = QUEUE_DATA(q, ReqStatus, node);
            req->expired = true;
        }
    }

    bool listen()
    {
        return uv_listen(handle<uv_stream_t>(), 128, stream_connection_cb)==0;
    }

    bool accept(Stream *client)
    {
        return uv_accept(handle<uv_stream_t>(), client->handle<uv_stream_t>())==0;
    }

    //Start read data from the connected endpoint.
    bool read_start()
    {
        return uv_read_start(handle<uv_stream_t>(), stream_alloc_cb, stream_read_cb)==0;
    }

    bool read_stop()
    {
        return uv_read_stop(handle<uv_stream_t>())==0;
    }

    struct stream_write_ctx
    {
        uv_write_t req;
        QByteArray buffer;
        WriteCallback callback;
        ReqStatus status;
    };

    //Write data on the stream
    bool write(const QByteArray &data, const WriteCallback &cb = nullptr)
    {
        stream_write_ctx *ctx = new stream_write_ctx;
        ctx->buffer = data;
        ctx->req.data = this;
        ctx->callback = cb;
        uv_buf_t buf;
        buf = uv_buf_init(ctx->buffer.data(), ctx->buffer.size());

        int err = uv_write(&ctx->req, handle<uv_stream_t>(), &buf, 1, stream_write_cb);
        if (err!=0)
        {
            delete ctx;
            return false;
        }
        registReqStatus(&ctx->status);
        return true;
    }

    struct stream_shutdown_ctx
    {
        uv_shutdown_t req;
        ShutdownCallback callback;
        ReqStatus status;
    };

    //Shutdown the write side of this Stream.
    bool shutdown(const ShutdownCallback &cb = nullptr)
    {
        stream_shutdown_ctx* ctx = new stream_shutdown_ctx;
        ctx->req.data = this;
        ctx->callback = cb;
        int err =  uv_shutdown(&ctx->req, handle<uv_stream_t>(), stream_shutdown_cb);
        if (err!=0)
        {
            delete ctx;
            return false;
        }
        registReqStatus(&ctx->status);
        return true;
    }

    //Indicates if stream is readable.
    bool readable()
    {
        return uv_is_readable(handle<uv_stream_t>());
    }

    //Indicates if stream is writable.
    bool writable()
    {
        return uv_is_writable(handle<uv_stream_t>());
    }   

    void onNewConnection(const NewConnectionCallback &cb)
    {
        m_onNewConnection = cb;
    }

    void onRead(const ReadCallback &cb)
    {
        m_onRead = cb;
    }

    void registReqStatus(ReqStatus* reqStatus)
    {
        reqStatus->expired = false;
        QUEUE_INSERT_TAIL(&m_reqStatusQueue, &reqStatus->node);
    }

    void unregistReqStatus(ReqStatus* reqStatus)
    {
        QUEUE_REMOVE(&reqStatus->node);
    }

private:
    DISABLE_COPY(Stream)
    static void stream_connection_cb(uv_stream_t* server, int /*status*/)
    {
        if (!server->data) return;
        Stream *_this = reinterpret_cast<Stream *>(server->data);
        if (_this->m_onNewConnection)
        {
            _this->m_onNewConnection();
        }
    }

    static void stream_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
    {
        //nread is > 0 if there is data available, 0 if libuv is done reading for now, or < 0 on error.
        if (!stream->data) return;
        Stream *_this = reinterpret_cast<Stream *>(stream->data);
        if (nread>0)
        {
            QByteArray ba(buf->base, nread);
            if (_this->m_onRead)
            {
                _this->m_onRead(ba);
            }
        }
        else if (nread<0)
        {
            _this->close();
        }
        Loop* loop = (Loop *)stream->loop->data;
        loop->buffer.in_use = false;
    }


    static void stream_alloc_cb(uv_handle_t* handle, size_t /*suggested_size*/, uv_buf_t* buf)
    {
        Loop* loop = (Loop *)handle->loop->data;

        if (loop->buffer.in_use)
        {
            buf->base = nullptr;
            buf->len = 0;
        }
        else{
            buf->base = loop->buffer.slab;
            buf->len = sizeof loop->buffer.slab;
            loop->buffer.in_use = true;
        }
    }

    static void stream_write_cb(uv_write_t* req, int status)
    {
        stream_write_ctx *ctx = (stream_write_ctx *)req;
        if (ctx->status.expired)
        {
            qDebug()<<"stream_write_cb ---------------expired-------------";
            return;
        }
        if (ctx->callback)
        {
            ctx->callback(status);
        }
        Stream *_this = (Stream *)req->data;
        _this->unregistReqStatus(&ctx->status);
        delete ctx;
    }

    static void stream_shutdown_cb(uv_shutdown_t* req, int /*status*/)
    {
        stream_shutdown_ctx *ctx = (stream_shutdown_ctx *)req;
        if (ctx->status.expired)
        {
            qDebug()<<"stream_shutdown_cb ---------------expired-------------";
            return;
        }
        if (ctx->callback)
        {
            ctx->callback();
        }
        Stream *_this = (Stream *)req->data;
        _this->unregistReqStatus(&ctx->status);
        delete ctx;
    }

    NewConnectionCallback m_onNewConnection;
    ReadCallback m_onRead;
    QUEUE m_reqStatusQueue;
};
}

#endif // UVPP_STREAM_H
