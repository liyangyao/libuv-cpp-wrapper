/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef STREAM_H
#define STREAM_H

#include "uvqt.h"
#include "loop.h"
#include "handle.h"

namespace uv{

//template <class Handle>
struct WriteRequest
{
public:
    WriteRequest(const QByteArray &ba, const RequestCallback &cb):
         m_byteArray(ba),
         m_writeCallback(cb)
    {
        m_buf.base = m_byteArray.data();
        m_buf.len = m_byteArray.length();
        m_req.data = this;
    }

    uv_write_t *ptr()
    {
        return &m_req;
    }

    const uv_buf_t *buf()
    {
        return &m_buf;
    }

    const RequestCallback& writeCallback()
    {
        return m_writeCallback;
    }

private:
    uv_write_t m_req;
    uv_buf_t m_buf;
    QByteArray m_byteArray;
    RequestCallback m_writeCallback;
};

template <class Type> class Stream: public Handle<Type>
{
public:
    explicit Stream():
        Handle<Type>()
    {
        m_stream = reinterpret_cast<uv_stream_t *>(ptr());
    }

    int read_start()
    {
        return uv_read_start(m_stream, on_alloc_cb, on_read_cb);
    }

    int write(const QByteArray &ba, const RequestCallback &cb)
    {
        WriteRequest *wr = new WriteRequest(ba, cb);
        return uv_write(wr->ptr(), m_stream, wr->buf(), 1, on_write_cb);
    }

    uv_stream_t* stream()
    {
        return m_stream;
    }

    void readyRead(QByteArray data)
    {
        qDebug()<<"readyRead:"<<data;
    }


private:
    uv_stream_t* m_stream;
    static void on_write_cb(uv_write_t* req, int status)
    {
        WriteRequest *wr = reinterpret_cast<WriteRequest *>(req->data);
        if (wr->writeCallback())
        {
            wr->writeCallback()(status);
        }
        delete wr;
    }

    static void on_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
    {
        static char BUFFER[65535];
        buf->base = BUFFER;
        buf->len = 65535;
    }

    static void on_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
    {
        //nread is > 0 if there is data available, 0 if libuv is done reading for now, or < 0 on error.
        qDebug()<<"nread="<<nread;
        Stream *_this = reinterpret_cast<Stream *>(stream->data);
        if (nread>0)
        {
            QByteArray ba(buf->base, nread);
            _this->readyRead(ba);
        }
        else if (nread<0)
        {
            _this->close();
        }

    }

};
}

#endif // STREAM_H
