/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef STREAM_H
#define STREAM_H

#include "uvqt.h"
#include "loop.h"
#include "handle.h"
#include "buf.h"

namespace uv{

//template <class Handle>
struct WrireReq
{

    WrireReq(const QByteArray &ba):
         m_byteArray(ba)
    {
        m_buf.base = m_byteArray.constData();
        m_buf.len = m_byteArray.length();

    }

    uv_write_t *ptr()
    {
        return &m_req;
    }

    uv_write_t m_req;
    uv_buf_t m_buf;
    QByteArray m_byteArray;
};

class Stream
{
public:
    explicit Stream(uv_stream_t *stream):
        m_stream(stream)
    {

    }

    int write(uv_write_t* req, const QByteArray &ba)
    {
        return uv_write(req, m_stream, bufs[], nbufs, on_write_cb);
    }

    uv_stream_t* stream()
    {
        return m_stream;
    }


private:
    uv_stream_t* m_stream;
    static void on_write_cb(uv_write_t* req, int status)
    {

    }

};
}

#endif // STREAM_H
