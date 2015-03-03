#include "echoserver.h"
#include <QDebug>

EchoServer::EchoServer(EventLoop *loop, int port):
    m_loop(loop),
    m_server(loop)
{
    m_server.setMessageCallback([](const TcpConnectionPtr& conn, const QByteArray& data)
    {
        qDebug()<<"Echo Begin";
        qDebug()<<data;
        qDebug()<<"Echo End";
        conn->send(data);
    });
    m_server.listen(port);
}
