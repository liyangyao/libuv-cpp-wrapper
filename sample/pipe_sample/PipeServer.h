/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/9/30

****************************************************************************/

#ifndef PIPESERVER_H
#define PIPESERVER_H

#include <unordered_map>
#include <uvpp/uvpp.h>

namespace uv{

typedef QVariant Context;

class PipeServer
{
public:
    typedef Pipe Client;
    typedef std::shared_ptr<Client> ClientPtr;
    typedef std::function<void(const ClientPtr &, QVariant *context)> CallbackNewConnection;
    explicit PipeServer(Loop* loop):
        m_loop(loop),
        m_server(loop, true),
        m_lastClientId(0)
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
    Client m_server;
    int m_lastClientId;
    std::unordered_map<int, ClientPtr> m_clients;
    void handleNewConnection()
    {
        int clientId = m_lastClientId++;
        ClientPtr client(new Client(m_loop, true));
        m_server.accept(client.get());
        client->read_start();
        Context* context = new QVariant;
        if (onConnection)
        {
            onConnection(client, context);
        }

        client->onClose([this, clientId, context]
        {
            m_clients.erase(clientId);
            delete context;
        });

        m_clients.insert(std::pair<int, ClientPtr>(clientId, client));
    }
    DISABLE_COPY(PipeServer)
};
}

#endif // PIPESERVER_H
