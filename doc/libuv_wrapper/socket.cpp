/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/2/27

****************************************************************************/
#pragma execution_character_set("utf-8")

#include "socket.h"
#include "eventloop.h"


namespace net
{
Socket::Socket(EventLoop *loop):
    m_loop(loop)
{
    uv_tcp_init(m_loop->handle(), &m_handle);
}

Socket::~Socket()
{
    uv_close((uv_handle_t *)&m_handle, nullptr);
}
}


