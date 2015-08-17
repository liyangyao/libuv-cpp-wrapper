/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#ifndef UVPP_STREAM_H
#define UVPP_STREAM_H

#include "uvpp_define.h"
#include "uvpp_handle.h"

namespace uvpp{
template <typename HANDLE_T>
class Stream: public Handle<HANDLE_T>
{
public:
    explicit Stream():
        Handle<HANDLE_T>()
    {
        handle()->data = this;
    }

    bool listen(const Callback &onConnection)
    {
        m_onConnection = onConnection;
        return uv_listen(handle<uv_stream_t>(), 128, on_connection_cb)==0;
    }

    bool accept(Stream *client)
    {
        return uv_accept(handle<uv_stream_t>(), client->handle<uv_stream_t>())>=0;
    }

    int read_start(MessageCallback onMessage)
    {
        m_onMessage = onMessage;
        return uv_read_start(handle<uv_stream_t>(), on_alloc_cb, on_read_cb);
    }

    bool read_stop()
    {
        return uv_read_stop(handle<uv_stream_t>())==0;
    }

//    int write(const QByteArray &ba, const RequestCallback &cb)
//    {
//        WriteRequest *wr = new WriteRequest(ba, cb);
//        return uv_write(wr->get(), handle<uv_stream_t>(), wr->buf(), 1, on_write_cb);
//    }

    int shutdown()
    {
        uv_shutdown_t* req = new uv_shutdown_t;
        return uv_shutdown(req, handle<uv_stream_t>(), on_shutdown_cb);
    }

    void onClose(const Callback &cb)
    {
        m_onClose = cb;
    }

private:
    Callback m_onConnection;
    Callback m_onClose;
    MessageCallback m_onMessage;

//    static void on_write_cb(uv_write_t* req, int status)
//    {
//        WriteRequest *wr = reinterpret_cast<WriteRequest *>(req->data);
//        if (wr->writeCallback())
//        {
//            wr->writeCallback()(status);
//        }
//        delete wr;
//    }

    static void on_alloc_cb(uv_handle_t* /*handle*/, size_t /*suggested_size*/, uv_buf_t* buf)
    {
        static char BUFFER[65535];
        buf->base = BUFFER;
        buf->len = 65535;
    }

    static void on_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
    {
        //nread is > 0 if there is data available, 0 if libuv is done reading for now, or < 0 on error.
        Stream *_this = reinterpret_cast<Stream *>(stream->data);
        if (nread>0)
        {
            QByteArray ba(buf->base, nread);
            if (_this->m_onMessage)
            {
                _this->m_onMessage(ba);
            }
        }
        else if (nread<0)
        {
            _this->read_stop();
            if (_this->m_onClose)
            {
                _this->m_onClose();
            }
        }
    }

    static void on_shutdown_cb(uv_shutdown_t* req, int /*status*/)
    {
        delete req;
    }

    static void on_connection_cb(uv_stream_t* server, int /*status*/)
    {
        Stream *_this = reinterpret_cast<Stream *>(server->data);
        if (_this->m_onConnection)
        {
            _this->m_onConnection();
        }
    }
    DISABLE_COPY(Stream)
};
}

#endif // UVPP_STREAM_H
