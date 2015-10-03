/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/18

****************************************************************************/

#ifndef UVPP_TCPSERVER_H
#define UVPP_TCPSERVER_H

#include <unordered_map>
#include <uvpp/uvpp.h>

namespace uv{

typedef QVariant Context;

class TcpServer
{
public:
    typedef Tcp Client;
    typedef std::shared_ptr<Client> ClientPtr;
    typedef std::function<void(const ClientPtr &, QVariant *context)> CallbackNewConnection;
    explicit TcpServer(Loop* loop):
        m_loop(loop),
        m_server(loop),
        m_lastClientId(0)
    {

    }

    bool listen(const char *ip, int port)
    {
        m_server.bind(ip, port);
        m_server.onConnection(std::bind(&TcpServer::handleNewConnection, this));
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
        ClientPtr client(new Client(m_loop));
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
    DISABLE_COPY(TcpServer)
};
}

#endif // UVPP_TCPSERVER_H
