/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/18

****************************************************************************/

#ifndef UVPP_TCPSERVER_H
#define UVPP_TCPSERVER_H

#include <vector>
#include <unordered_map>
#include <uvpp/uvpp.h>



namespace uv{

typedef std::shared_ptr<Tcp> TcpPtr;
typedef QVariant Context;
typedef std::function<void(const TcpPtr &, QVariant *context)> CallbackWithTcp;
class TcpServer
{
public:
    explicit TcpServer(Loop* loop):
        m_loop(loop),
        m_tcpServer(loop),
        m_lastClientId(0)
    {

    }

    bool listen(const char *ip, int port)
    {
        m_tcpServer.bind(ip, port);
        m_tcpServer.onConnection(std::bind(&TcpServer::handleNewConnection, this));
        return m_tcpServer.listen();
    }

    CallbackWithTcp onConnection;
private:
    Loop* m_loop;
    Tcp m_tcpServer;
    int m_lastClientId;
    std::unordered_map<int, TcpPtr> m_clients;
    void handleNewConnection()
    {
        int clientId = m_lastClientId++;
        TcpPtr tcp(new Tcp(m_loop));
        m_tcpServer.accept(tcp.get());        
        tcp->read_start();
        Context* context = new QVariant;
        if (onConnection)
        {
            onConnection(tcp, context);
        }

        tcp->onClose([this, clientId, context]
        {
            m_clients.erase(clientId);
            delete context;
        });

        m_clients.insert(std::pair<int, TcpPtr>(clientId, tcp));
    }

    DISABLE_COPY(TcpServer)
};
}

#endif // UVPP_TCPSERVER_H
