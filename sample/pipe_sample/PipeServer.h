/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/9/30

****************************************************************************/

#ifndef PIPESERVER_H
#define PIPESERVER_H

#include <uvpp/uvpp_pipe.h>


namespace uv{


class PipeConnection
{
public:
    PipeConnection(Loop* loop):
        m_loop(loop),
        m_pipe(loop, true)
    {

    }

    ~PipeConnection()
    {

    }

    void setContext(const std::shared_ptr<void> &context)
    {
        m_context = context;
    }

    Pipe* pipe()
    {
        return &m_pipe;
    }

    Loop* loop()
    {
        return m_loop;
    }

private:
    std::shared_ptr<void> m_context;
    Pipe m_pipe;
    Loop* m_loop;
};
typedef std::shared_ptr<PipeConnection> PipeConnectionPtr;


class PipeServer
{
public:
    typedef std::function<void(const PipeConnectionPtr &)> CallbackNewConnection;
    explicit PipeServer(Loop* loop):
        m_loop(loop),
        m_server(loop, true)
    {

    }

    bool listen(const char *name)
    {
        m_server.bind(name);
        m_server.onConnection(std::bind(&PipeServer::handleNewConnection, this));
        return m_server.listen();
    }

    CallbackNewConnection onConnection;
private:
    Loop* m_loop;
    Pipe m_server;

    void handleNewConnection()
    {
        PipeConnectionPtr conn(new PipeConnection(m_loop));
        m_server.accept(conn->pipe());
        conn->pipe()->read_start();

        if (onConnection)
        {
            onConnection(conn);
        }

        conn->pipe()->onClose([conn]
        {
            conn->setContext(nullptr);
            conn->pipe()->onClose(nullptr);
        });
    }
    DISABLE_COPY(PipeServer)
};
}


#endif // PIPESERVER_H
