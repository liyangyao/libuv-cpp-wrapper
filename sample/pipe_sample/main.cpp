/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/9/30

****************************************************************************/

#include "widget.h"
#include <QApplication>
#include "PipeServer.h"

class Session
{
public:
    Session(const uv::PipeServer::ClientPtr &client, uv::Context *context):
        m_client(client),
        m_context(context)
    {
        client->onData(std::bind(&Session::onData, this, std::placeholders::_1, std::placeholders::_2));
    }

private:
    uv::PipeServer::ClientPtr m_client;
    uv::Context* m_context;
    void onData(const char *data, int size)
    {
        QByteArray ba(data, size);
        m_client->write(ba);
    }

};
typedef std::shared_ptr<Session> SessionPtr;
Q_DECLARE_METATYPE(SessionPtr)

void pipeServerThread()
{
    uv::Thread thread([]()
    {
        uv::Loop loop;
        uv::PipeServer svr(&loop);
        svr.listen("HelloWorld");
        svr.onConnection=[](const uv::PipeServer::ClientPtr &client, uv::Context *context)
        {
            SessionPtr session(new Session(client, context));
            context->setValue(session);
        };
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
