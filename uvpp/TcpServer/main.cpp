/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/18

****************************************************************************/

#include <QCoreApplication>
#include "libuvpp.h"
#include <QDebug>
#include <unordered_map>
#include <queue>
#include <QTextCodec>
#include <QUuid>
//#include "any.h"




class LoopEx:public Loop
{
public:
    LoopEx():
        Loop()
    {
        uv_mutex_init(&m_functor_mutex);
        m_functor_async.get()->data = this;
        uv_async_init(handle(), m_functor_async.get(), functor_async_cb);
    }

    void queueInLoop(const Functor &functor)
    {
        uv_mutex_lock(&m_functor_mutex);
        m_functors.push(functor);
        uv_mutex_unlock(&m_functor_mutex);
        uv_async_send(m_functor_async.get());
    }


private:
    std::queue<Functor> m_functors;
    Handle<uv_async_t> m_functor_async;
    uv_mutex_t m_functor_mutex;
    void onFunctor()
    {
        while (true)
        {
            Functor fun = nullptr;
            uv_mutex_lock(&m_functor_mutex);
            if (!m_functors.empty())
            {
                fun.swap(m_functors.front());
                m_functors.pop();
            }
            uv_mutex_unlock(&m_functor_mutex);
            if (fun)
            {
                fun();
            }
            else{
                break;
            }
        }
    }

    static void functor_async_cb(uv_async_t* handle)
    {
        qDebug()<<"functor_async_cb";
        LoopEx *_this = reinterpret_cast<LoopEx *>(handle->data);
        _this->onFunctor();
    }
};

class Connection:public std::enable_shared_from_this<Connection>
{
public:
    typedef std::shared_ptr<Connection> Ptr;
    typedef std::function<void(const Ptr &)> Callback;
    typedef std::function<void(const Ptr &, const QByteArray &)> MessageCallback;

    Connection(Loop *loop):
        m_tcp(loop)
    {
        static int id = 0;
        id++;
        m_id = id;
        qDebug()<<"Connection"<<m_id;

        m_tcp.setCloseCallback(std::bind(&Connection::handleClose, this));

    }

    ~Connection()
    {
        qDebug()<<"~Connection"<<m_id;
    }

    int id()
    {
        return m_id;
    }

    Tcp* getTcp()
    {
        return &m_tcp;
    }

    void connectionEstablished()
    {
        m_tcp.read_start(std::bind(&Connection::handleRead, this, std::placeholders::_1));
    }

    void connectionDestroyed()
    {

    }



    Callback closeCallback;
    MessageCallback messageCallback;

    std::shared_ptr<void> context;
private:
    Tcp m_tcp;
    int m_id;
    void handleClose()
    {
        qDebug()<<"handleClose";
        Ptr guardThis(shared_from_this());
        closeCallback(guardThis);
    }

    void handleRead(const QByteArray &data)
    {
        if (messageCallback)
        {
            messageCallback(shared_from_this(), data);
        }
    }
};
typedef std::shared_ptr<Connection> ConnectionPtr;

class Session
{
public:
    Session()
    {
        uuid = QUuid::createUuid().toString();
        round = 0;
        qDebug()<<"Session:"<<uuid;
    }

    ~Session()
    {
        qDebug()<<"~Session"<<uuid;
    }

    int round;
    QString uuid;
};


class TcpServer
{
public:
    TcpServer(LoopEx* loop):
        m_loop(loop),
        m_tcpServer(loop)
    {

    }
    bool listen(const char *ip, int port)
    {
        m_tcpServer.bind(ip, port);
        return m_tcpServer.listen(std::bind(&TcpServer::onNewConnection, this, std::placeholders::_1));
    }



    typedef std::function<void(const ConnectionPtr &)> ConnectionCallback;
    ConnectionCallback connectionCallback;
    ConnectionCallback connectionCloseCallback;
    Connection::MessageCallback messageCallback;

private:
    LoopEx* m_loop;
    Tcp m_tcpServer;
    std::unordered_map<int, ConnectionPtr> m_connections;


    void onNewConnection(int status)
    {
        ConnectionPtr connection(new Connection(m_loop));
        m_tcpServer.accept(connection->getTcp());
        connection->connectionEstablished();

        m_connections.insert(std::make_pair(connection->id(), connection));
        connection->closeCallback = std::bind(&TcpServer::onConnectionClose, this, std::placeholders::_1);
        connection->messageCallback = messageCallback;
        qDebug()<<"onNewConnection";
        if (connectionCallback)
        {
            connectionCallback(connection);
        }
    }

    void onConnectionClose(const ConnectionPtr &connection)
    {
        m_loop->queueInLoop([connection]()
        {
            qDebug()<<"Loop destroy connection";
            connection->connectionDestroyed();
        });
        m_connections.erase(connection->id());
        qDebug()<<"onConnectionClose";
        if (connectionCloseCallback)
        {
            connectionCloseCallback(connection);
        }
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

    LoopEx loop;
    TcpServer server(&loop);
    server.listen("0.0.0.0", 80);
    server.messageCallback = ([](const ConnectionPtr &connection, const QByteArray &d)
    {
        std::shared_ptr<Session> session = std::static_pointer_cast<Session>(connection->context);
        session->round++;
        QByteArray respon = QString("%1 round=%2").arg(session->uuid).arg(session->round).toUtf8();
        QString head = QString("HTTP/1.1 200 OK\r\nContent-Length: %1\r\n\r\n").arg(respon.size());
        connection->getTcp()->write(head.toUtf8(), nullptr);
        connection->getTcp()->write(respon, nullptr);
    });
    server.connectionCallback = [](const ConnectionPtr &connection)
    {
        connection->context = std::shared_ptr<Session>(new Session);
        qDebug()<<"connectionCallback";
    };

    server.connectionCloseCallback = [](const ConnectionPtr &connection)
    {
        std::shared_ptr<Session> session = std::static_pointer_cast<Session>(connection->context);

        qDebug()<<"Session Close"<<session->uuid;
    };

    qDebug()<<"begin run";
    loop.run();

    qDebug()<<"OVER";
    return a.exec();
}
