/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/21

****************************************************************************/

#ifndef GETADDRINFO_H
#define GETADDRINFO_H
#include "uvpp.h"
#include "loop.h"

namespace uv{
class Dns
{
public:
    explicit Dns()
    {
        hints.ai_family = PF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = 0;
        resolver.data = this;
        m_addrinfo = nullptr;
    }

    ~Dns()
    {
        freeAddrInfo();
    }

    int resolve(Loop *loop, const char *node, const char *service)
    {
        return uv_getaddrinfo(loop->handle(), &resolver, on_resolved, node,
                              service, NULL);
    }



private:
    struct addrinfo hints;
    uv_getaddrinfo_t resolver;
    addrinfo *m_addrinfo;
    void freeAddrInfo()
    {
        if (m_addrinfo)
        {
            uv_freeaddrinfo(m_addrinfo);
            m_addrinfo = nullptr;
        }
    }

    static void on_resolved(uv_getaddrinfo_t *resolver, int status,
                            struct addrinfo *info)
    {
        Dns *_this = reinterpret_cast<Dns *>(resolver->data);
        _this->freeAddrInfo();
        _this->m_addrinfo = info;

        char addr[17] = {'\0'};
        uv_ip4_name((struct sockaddr_in*) info->ai_addr, addr, 16);
        qDebug()<<addr;

//        uv_tcp_connect(connect_req, socket, *(struct sockaddr_in*) info->ai_addr, on_connect);


    }
    DISABLE_COPY(Dns)

};
}

#endif // GETADDRINFO_H
