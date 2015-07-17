/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef TCP_H
#define TCP_H

#include "uvpp.h"
#include "loop.h"
#include "handle.h"
#include "stream.h"
#include <QDebug>

namespace uv{

template <typename ContextType>
class Tcp: public Stream<uv_tcp_t>
{
public:
    explicit Tcp(Loop *loop):
        Stream<uv_tcp_t>()
    {
        uv_tcp_init(loop->handle(), get());
    }

    ~Tcp()
    {
        qDebug()<<"~Tcp";
    }

    int nodelay(int enable)
    {
        return uv_tcp_nodelay(get(), enable);
    }

    int keepalive(int enable, unsigned int delay)
    {
        return uv_tcp_keepalive(get(), enable, delay);
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
        return uv_tcp_connect(m_connect_req.get(), get(),
                              reinterpret_cast<const struct sockaddr *>(&addr),
                              on_connect_cb);
    }

    int bind(const char *ip, int port)
    {
        struct sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        return uv_tcp_bind(get(), reinterpret_cast<const struct sockaddr *>
                           (&addr), 0);
    }

    ContextType context()
    {
        return m_context;
    }

    void setContext(const ContextType &context)
    {
        m_context = context;
    }

private:
    unique_ptr<uv_connect_t> m_connect_req;
    RequestCallback m_connectCallback;
    ContextType m_context;

    static void on_connect_cb(uv_connect_t* req, int status)
    {
        Tcp *_this = reinterpret_cast<Tcp *>(req->data);
        if (_this->m_connectCallback)
        {
            _this->m_connectCallback(status);
        }
    }
    DISABLE_COPY(Tcp)
};
}

#endif // TCP_H