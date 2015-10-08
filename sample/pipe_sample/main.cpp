/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/9/30

****************************************************************************/

#include "widget.h"
#include <QApplication>
#include "PipeServer.h"
#include <uvpp/uvpp.h>

class Session
{
public:
    Session(const uv::PipeConnectionPtr &client):
        m_client(client)
    {
        client->pipe()->onData(std::bind(&Session::onData, this, std::placeholders::_1, std::placeholders::_2));
    }

private:
    uv::PipeConnectionPtr m_client;
    void onData(const char *data, int size)
    {
        QByteArray ba(data, size);
        m_client->pipe()->write(ba);
    }

};
typedef std::shared_ptr<Session> SessionPtr;


void pipeServerThread()
{
    uv::Thread* thread = new uv::Thread([]()
    {
        uv::Loop loop;
        uv::PipeServer svr(&loop);

        svr.onConnection=[](const uv::PipeConnectionPtr &client)
        {
            qDebug()<<"Server::onConnection";
            SessionPtr session(new Session(client));
            client->setContext(session);
        };
        svr.listen("HelloWorld");
        uv::Timer timer(&loop);
        timer.start([&loop]()
        {
            qDebug()<<"begin";
            uv::Pipe* pipe = new uv::Pipe(&loop, true);
            pipe->connect("HelloWorld", [&pipe](int status)
            {
                qDebug()<<"after connect status="<<status;
                pipe->write("HelloWorld!!!");
            });

        }, 1000, 0);
        loop.run();
    });
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    pipeServerThread();

    return a.exec();
}
