/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/29

****************************************************************************/

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QDebug>
#include <unordered_map>
#include <deque>
#include "libuvpp.h"

namespace uv
{

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::weak_ptr<Connection> ConnectionWeakPtr;
typedef std::function<void(const ConnectionPtr &)> ConnectionCallback;
typedef std::function<void(const ConnectionPtr &)> ConnectionCloseCallback;
typedef std::function<void(const ConnectionPtr &, const QByteArray &)> ConnectionMessageCallback;

class Buffer
{
    class BufferData
    {
    public:
        explicit BufferData()
        {
            readerIndex_ = 0;
            writerIndex_ = 0;
        }

        explicit BufferData(const char* data, size_t len)
        {
            readerIndex_ = 0;
            writerIndex_ = 0;
            append(data, len);
        }

        size_t writableBytes() const
        {
            return buffer_.size() - writerIndex_;
        }

        size_t readableBytes() const
        {
            return writerIndex_ - readerIndex_;
        }

        void ensureWritableBytes(size_t len)
        {
            if (writableBytes() < len)
            {
                makeSpace(len);
            }
            Q_ASSERT(writableBytes() >= len);
        }

        char* beginWrite()
        {
            return begin() + writerIndex_;
        }

        const char* beginWrite() const
        {
            return begin() + writerIndex_;
        }

        void hasWritten(size_t len)
        {
            Q_ASSERT(len <= writableBytes());
            writerIndex_ += len;
        }

        void unwrite(size_t len)
        {
            Q_ASSERT(len <= readableBytes());
            writerIndex_ -= len;
        }

        void append(const char* /*restrict*/ data, size_t len)
        {
            ensureWritableBytes(len);
            std::copy(data, data+len, beginWrite());
            hasWritten(len);
        }

        const char* peek()
        {
            return begin();
        }

        template<typename T>
        T peek()
        {
            static_assert(std::is_integral<T>::value, "integral required.");
            T* p = reinterpret_cast<T*>(begin());
            return *p;
        }

        template<typename T>
        T read()
        {
            static_assert(std::is_integral<T>::value, "integral required.");
            T* p = reinterpret_cast<T*>(begin());
            retrieve(sizeof T);
            return *p;
        }

        void retrieve(size_t len)
        {
            if (len < readableBytes())
            {
                readerIndex_ += len;
            }
            else
            {
                retrieveAll();
            }
        }

        void retrieveAll()
        {
            readerIndex_ = 0;
            writerIndex_ = 0;
        }

        QString retrieveAsString(size_t len)
        {
            Q_ASSERT(len <= readableBytes());
            QString result = QString::fromUtf8(peek(), len);
            retrieve(len);
            return result;
        }

        QString retrieveAsString()
        {
            return retrieveAsString(readableBytes());
        }


    private:
        char* begin()
        {
            return &*buffer_.begin();
        }

        const char* begin() const
        {
            return &*buffer_.begin();
        }

        void makeSpace(size_t len)
        {
            if (writableBytes()< len)
            {
                // FIXME: move readable data
                buffer_.resize(writerIndex_+len);
            }
            else
            {
                // move readable data to the front, make space inside buffer
                size_t readable = readableBytes();
                std::copy(begin()+readerIndex_,
                          begin()+writerIndex_,
                          begin());
                readerIndex_ = 0;
                writerIndex_ = readerIndex_ + readable;
            }
        }
    private:
        std::vector<char> buffer_;
        size_t readerIndex_;
        size_t writerIndex_;

    };

    typedef std::shared_ptr<BufferData> BufferDataPtr;
public:
    explicit Buffer():
        m_ptr(new BufferData)
    {

    }

    Buffer(const QByteArray& data)
    {
        qDebug()<<"Buffer QByteArray";
        m_ptr = std::make_shared<BufferData>(data.constData(), data.size());
    }
    Buffer(const QString& string)
    {
        qDebug()<<"Buffer QString";
        QByteArray data = string.toUtf8();
        m_ptr = std::make_shared<BufferData>(data.constData(), data.size());
    }
    Buffer(const char *ch, int size = -1)
    {
        qDebug()<<"Buffer char*";
        if (size<0)
        {
            size = strlen(ch);
        }
        m_ptr = std::make_shared<BufferData>(ch, size);
    }
    Buffer(const Buffer& b):
        m_ptr(b.m_ptr)
    {
        //qDebug()<<"Buffer Copy";
    }
    Buffer(Buffer&& b)
    {
        m_ptr.swap(b.m_ptr);
        //qDebug()<<"Buffer Copy RightValue";
    }

    const BufferDataPtr& data()
    {
        return m_ptr;
    }

    BufferData* const data2() //const
    {
        return m_ptr.get();
    }

    BufferData* const operator ->() const
    {
        return m_ptr.get();
    }

private:
    BufferDataPtr m_ptr;
};
}

class AsyncFunctor
{
public:
    explicit AsyncFunctor(Loop* loop)
    {
        m_async.reset(new Async(loop, std::bind(&AsyncFunctor::onFunctor, this)));
    }

    void queue(const Callback &functor)
    {
        MutexLocker lock(&m_mutex);
        m_functors.push_back(functor);
        m_async->send();
    }

private:
    std::deque<Callback> m_functors;
    std::unique_ptr<Async> m_async;
    Mutex m_mutex;
    void onFunctor()
    {
        while (true)
        {
            Callback fun = nullptr;
            {
                MutexLocker lock(&m_mutex);
                if (!m_functors.empty())
                {
                    fun.swap(m_functors.front());
                    m_functors.pop_front();
                }
            }

            if (fun)
            {
                fun();
            }
            else{
                break;
            }
        }
    }
    DISABLE_COPY(AsyncFunctor)
};


class Connection:public std::enable_shared_from_this<Connection>
{
public:
    explicit Connection(Loop *loop):
        m_loop(loop),
        m_tcp(loop)
    {
        qDebug()<<"Connection Constructor("<<this<<")";
        static int id = 0;
        id++;
        m_id = id;                    
    }

    ~Connection()
    {
        qDebug()<<"Connection Destructor("<<this<<")";
    }

    int id()
    {
        return m_id;
    }

    void shutdown()
    {
        m_tcp.shutdown();
    }

    void forceClose()
    {
        handleClose();
    }

    void write(const QByteArray &d)
    {
        m_tcp.write(d, nullptr);
    }

    Loop* loop()
    {
        return m_loop;
    }

    ConnectionMessageCallback messageCallback;
    std::shared_ptr<void> context;

private:
    void connectionEstablished()
    {
        m_tcp.onClose(std::bind(&Connection::handleClose, this));
        m_tcp.read_start(std::bind(&Connection::handleRead, this, std::placeholders::_1));
    }

    void connectionDestroyed()
    {                
        qDebug()<<"Connection connectionDestroyed("<<this<<")";
        context = nullptr;
    }

    ConnectionCallback closeCallback;
    friend class TcpServer;

private:
    Loop* m_loop;
    Tcp m_tcp;
    int m_id;
    void handleClose()
    {
        qDebug()<<"Connection handleClose("<<this<<")";
        ConnectionPtr guardThis(shared_from_this());        
        closeCallback(guardThis);
    }

    void handleRead(const QByteArray &data)
    {
        //qDebug()<<"Connection handleRead("<<this<<")";
        if (messageCallback)
        {
            messageCallback(shared_from_this(), data);
        }
    }
    DISABLE_COPY(Connection)
};

class TcpServer
{
public:
    explicit TcpServer(Loop* loop):
        m_loop(loop),
        m_tcpServer(loop),
        m_asyncFunctor(loop)
    {

    }
    bool listen(const char *ip, int port)
    {
        m_tcpServer.bind(ip, port);
        return m_tcpServer.listen(std::bind(&TcpServer::handleConnection, this));
    }

    ConnectionCallback onConnection;
    ConnectionCloseCallback onConnectionClose;
    ConnectionMessageCallback onMessage;
private:
    Loop* m_loop;
    Tcp m_tcpServer;
    AsyncFunctor m_asyncFunctor;
    std::unordered_map<int, ConnectionPtr> m_connections;

    void handleConnection()
    {
        ConnectionPtr connection(new Connection(m_loop));
        m_tcpServer.accept(&connection->m_tcp);
        connection->connectionEstablished();

        m_connections.insert(std::make_pair(connection->id(), connection));
        connection->closeCallback = std::bind(&TcpServer::hanleConnectionClose, this, std::placeholders::_1);
        connection->messageCallback = onMessage;
        //qDebug()<<"onNewConnection";
        if (onConnection)
        {
            onConnection(connection);
        }
    }

    void hanleConnectionClose(const ConnectionPtr &connection)
    {
        m_asyncFunctor.queue(std::bind(&Connection::connectionDestroyed, connection));
        m_connections.erase(connection->id());
        if (onConnectionClose)
        {
            onConnectionClose(connection);
        }
    }
    DISABLE_COPY(TcpServer)
};
}
#endif // TCPSERVER_H
