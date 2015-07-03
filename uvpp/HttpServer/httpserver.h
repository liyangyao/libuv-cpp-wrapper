/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/3

****************************************************************************/

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <uvpp.h>
#include <uvpp/net/loop.h>
#include <uvpp/net/tcp.h>

class HttpServer
{
public:
    HttpServer();
    void start(int port);

private:
    uv::Loop m_loop;
    uv::Tcp<HttpServer *> m_tcpServer;
    void onNewConnection(int status);
};

#endif // HTTPSERVER_H
