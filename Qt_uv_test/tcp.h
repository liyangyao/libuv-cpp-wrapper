/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef TCP_H
#define TCP_H

#include "uvqt.h"
#include "loop.h"
#include "handle.h"
#include "stream.h"

namespace uv{

class Tcp: public Stream<uv_tcp_t>
{
public:
    explicit Tcp(Loop *loop):
        Stream<uv_tcp_t>()
    {
        uv_tcp_init(loop->handle(), ptr());
    }

    int nodelay(int enable)
    {
        return uv_tcp_nodelay(ptr(), enable);
    }

    int keepalive(int enable, unsigned int delay)
    {
        return uv_tcp_keepalive(ptr(), enable, delay);
    }

    int connect(const char *ip, int port, const RequestCallback &cb)
    {
        struct sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        if (!m_connect_req)
        {
            m_connect_req.reset(new uv_connect_t);
            m_connect_req->data = this;
        }
        m_connectCallback = cb;
        return uv_tcp_connect(m_connect_req.get(), ptr(), reinterpret_cast<const struct sockaddr *>(&addr), on_connect_cb);
    }

private:
    unique_ptr<uv_connect_t> m_connect_req;
    RequestCallback m_connectCallback;

    static void on_connect_cb(uv_connect_t* req, int status)
    {
        Tcp *_this = reinterpret_cast<Tcp *>(req->data);
        if (_this->m_connectCallback)
        {
            _this->m_connectCallback(status);
        }
    }
};
}

#endif // TCP_H
