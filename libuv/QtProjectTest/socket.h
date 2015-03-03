/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/2/27

Socket是一个RAII uv_tcp_t
生命周期由TcpConnection控制
****************************************************************************/

#ifndef SOCKET_H
#define SOCKET_H

#include <uv.h>

class EventLoop;
namespace net
{
class Socket
{
public:
    explicit Socket(EventLoop *loop);
    ~Socket();
    uv_tcp_t *handle() { return &m_handle; }


private:
    uv_tcp_t m_handle;
    EventLoop *m_loop;

};
}

#endif // SOCKET_H
