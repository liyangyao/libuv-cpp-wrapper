#ifndef UVPP_TCP_H
#define UVPP_TCP_H

#include "uvpp_define.h"
#include "uvpp_handle.h"
#include "uvpp_loop.h"
#include "uvpp_stream.h"

namespace uv{

class Tcp: public Stream<uv_tcp_t>
{
public:
    static int TcpCount;
    explicit Tcp(Loop *loop):
        Stream<uv_tcp_t>()
    {
        TcpCount++;
        qDebug()<<"Tcp create("<<this<<")"<<TcpCount;
        uv_tcp_init(loop->handle(), handle());
    }

    ~Tcp()
    {
        TcpCount--;
        qDebug()<<"~Tcp destructor("<<this<<")"<<TcpCount;
    }

    //Bind to the specified IP and port.
    bool bind(const char *ip, int port)
    {
        struct sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        return uv_tcp_bind(handle(), reinterpret_cast<const struct sockaddr *>
                           (&addr), 0)==0;
    }

    bool nodelay(int enable)
    {
        return uv_tcp_nodelay(handle(), enable)==0;
    }

    bool keepalive(int enable, unsigned int delay)
    {
        return uv_tcp_keepalive(handle(), enable, delay)==0;
    }

    struct tcp_connect_ctx
    {
        uv_connect_t req;
        CallbackWithResult callback;
    };

    //Start connecion to remote endpoint.
    bool connect(const char *ip, int port, const CallbackWithResult &connectCallback)
    {
        struct sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        tcp_connect_ctx *ctx = new tcp_connect_ctx;
        ctx->callback = connectCallback;

        int err = uv_tcp_connect(&ctx->req, handle(),
                              reinterpret_cast<const struct sockaddr *>(&addr),
                              tcp_connect_cb);
        if (err!=0)
        {
            delete ctx;
            return false;
        }
        return true;
    }

    QString getPeerName()
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
    DISABLE_COPY(Tcp)
    static void tcp_connect_cb(uv_connect_t* req, int status)
    {
        tcp_connect_ctx *ctx = UV_CONTAINER_OF(req, tcp_connect_ctx, req);
        if (ctx->callback)
        {
            ctx->callback(status);
        }
        delete ctx;
    }
};
int Tcp::TcpCount = 0;
}

#endif // UVPP_TCP_H
