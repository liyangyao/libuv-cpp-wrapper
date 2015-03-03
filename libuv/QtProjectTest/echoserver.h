#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include "tcpserver.h"

class EchoServer
{
public:
    EchoServer(EventLoop* loop, int port);

private:
    TcpServer m_server;
    EventLoop* m_loop;
};

#endif // ECHOSERVER_H
