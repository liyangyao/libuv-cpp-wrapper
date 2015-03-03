#ifndef CHARGENSERVER_H
#define CHARGENSERVER_H

#include "tcpserver.h"

class ChargenServer
{
public:
    ChargenServer(EventLoop* loop, int port);

private:
    TcpServer m_server;
    EventLoop* m_loop;
    QByteArray m_message;
    void onConnection(const TcpConnectionPtr& conn);
    void onWriteComplete(const TcpConnectionPtr& conn);
};

#endif // CHARGENSERVER_H
