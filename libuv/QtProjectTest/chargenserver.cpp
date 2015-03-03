#include "chargenserver.h"
#include <QDebug>

ChargenServer::ChargenServer(EventLoop* loop, int port):
    m_loop(loop),
    m_server(loop)
{
      QByteArray line;
      for (int i = 33; i < 127; ++i)
      {
        line.append(char(i));
      }
      line.append(line);
      for (size_t i = 0; i < 127-33; ++i)
      {
          m_message.append(line.mid(i, 72) + "\n");
      }

    m_server.setConnectionCallback(std::bind(&ChargenServer::onConnection, this, std::placeholders::_1));
    m_server.setWriteCompleteCallback(std::bind(&ChargenServer::onWriteComplete, this, std::placeholders::_1));
    m_server.listen(port);
}

void ChargenServer::onConnection(const TcpConnectionPtr &conn)
{
    //qDebug()<<m_message;
    qDebug()<<"ChargenServer::onConnection";
    conn->send(m_message);
}

void ChargenServer::onWriteComplete(const TcpConnectionPtr &conn)
{
    qDebug()<<"onWriteComplete";
    static int round = 0;
    round++;
    if (round>=0)
    {
        conn->shutdown();
        return;
    }
    conn->send(m_message);
}
