#ifndef UVPP_TCP_H
#define UVPP_TCP_H

#include "uvpp_define.h"
#include "uvpp_handle.h"
#include "uvpp_loop.h"
#include "uvpp_stream.h"

namespace uvpp{

class Tcp: public Stream<uv_tcp_t>
{
public:
    explicit Tcp(Loop *loop):
        Stream<uv_tcp_t>()
    {
        //qDebug()<<"Tcp";
        uv_tcp_init(loop->handle(), handle());
    }

    ~Tcp()
    {
        //qDebug()<<"~Tcp";
    }

    //Bind to the specified IP and port.
    int bind(const char *ip, int port)
    {
        struct sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        return uv_tcp_bind(handle(), reinterpret_cast<const struct sockaddr *>
                           (&addr), 0);
    }

    int nodelay(int enable)
    {
        return uv_tcp_nodelay(handle(), enable);
    }

    int keepalive(int enable, unsigned int delay)
    {
        return uv_tcp_keepalive(handle(), enable, delay);
    }

    //Start connecion to remote endpoint.
    int connect(const char *ip, int port, const ConnectCallback &onConnect)
    {
        m_onConnect = onConnect;
        struct sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        if (!m_connect_req)
        {
            m_connect_req.reset(new uv_connect_t);
            m_connect_req->data = this;
        }
        return uv_tcp_connect(m_connect_req.get(), handle(),
                              reinterpret_cast<const struct sockaddr *>(&addr),
                              on_connect_cb);
    }

    QString getsockname()
    {
        struct sockaddr_in name;
        int namelen = sizeof(name);
        uv_tcp_getsockname(handle(), (struct sockaddr*) &name, &namelen);

        char addr[16];
        uv_inet_ntop(AF_INET, &name.sin_addr, addr, sizeof(addr));

        QString sockname = addr;
        return sockname;
    }

    QString getpeername()
    {
        struct sockaddr_in name;
        int namelen = sizeof(name);
        uv_tcp_getpeername(handle(), (struct sockaddr*) &name, &namelen);

        char addr[16];
        uv_inet_ntop(AF_INET, &name.sin_addr, addr, sizeof(addr));

        QString peername = addr;
        return peername;
    }



private:
    unique_ptr<uv_connect_t> m_connect_req;
    ConnectCallback m_onConnect;

    static void on_connect_cb(uv_connect_t* req, int status)
    {
        bool connected = status==0;
        Tcp *_this = reinterpret_cast<Tcp *>(req->data);
        if (_this->m_onConnect)
        {
            _this->m_onConnect(connected);
        }
    }
    DISABLE_COPY(Tcp)
};
}

#endif // UVPP_TCP_H
