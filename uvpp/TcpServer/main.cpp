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
//#include "any.h"


namespace cdiggins
{
namespace anyimpl
{
struct bad_any_cast
{
};

struct empty_any
{
};

struct base_any_policy
{
    virtual void static_delete(void** x) = 0;
    virtual void copy_from_value(void const* src, void** dest) = 0;
    virtual void clone(void* const* src, void** dest) = 0;
    virtual void move(void* const* src, void** dest) = 0;
    virtual void* get_value(void** src) = 0;
    virtual size_t get_size() = 0;
};

template<typename T>
struct typed_base_any_policy : base_any_policy
{
    virtual size_t get_size() { return sizeof(T); }
};

template<typename T>
struct small_any_policy : typed_base_any_policy<T>
{
    virtual void static_delete(void** x) { }
    virtual void copy_from_value(void const* src, void** dest)
    { new(dest) T(*reinterpret_cast<T const*>(src)); }
    virtual void clone(void* const* src, void** dest) { *dest = *src; }
    virtual void move(void* const* src, void** dest) { *dest = *src; }
    virtual void* get_value(void** src) { return reinterpret_cast<void*>(src); }
};

template<typename T>
struct big_any_policy : typed_base_any_policy<T>
{
    virtual void static_delete(void** x) { if (*x)
            delete(*reinterpret_cast<T**>(x)); *x = NULL; }
    virtual void copy_from_value(void const* src, void** dest) {
        *dest = new T(*reinterpret_cast<T const*>(src)); }
    virtual void clone(void* const* src, void** dest) {
        *dest = new T(**reinterpret_cast<T* const*>(src)); }
    virtual void move(void* const* src, void** dest) {
        (*reinterpret_cast<T**>(dest))->~T();
        **reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src); }
    virtual void* get_value(void** src) { return *src; }
};

template<typename T>
struct choose_policy
{
    typedef big_any_policy<T> type;
};

template<typename T>
struct choose_policy<T*>
{
    typedef small_any_policy<T*> type;
};

struct any;

/// Choosing the policy for an any type is illegal, but should never happen.
/// This is designed to throw a compiler error.
template<>
struct choose_policy<any>
{
    typedef void type;
};

/// Specializations for small types.
#define SMALL_POLICY(TYPE) template<> struct choose_policy<TYPE> { typedef small_any_policy<TYPE> type; };

SMALL_POLICY(signed char);
SMALL_POLICY(unsigned char);
SMALL_POLICY(signed short);
SMALL_POLICY(unsigned short);
SMALL_POLICY(signed int);
SMALL_POLICY(unsigned int);
SMALL_POLICY(signed long);
SMALL_POLICY(unsigned long);
SMALL_POLICY(float);
SMALL_POLICY(bool);

#undef SMALL_POLICY

/// This function will return a different policy for each type.
template<typename T>
base_any_policy* get_policy()
{
    static typename choose_policy<T>::type policy;
    return &policy;
};
}

struct any
{
private:
    // fields
    anyimpl::base_any_policy* policy;
    void* object;

public:
    /// Initializing constructor.
    template <typename T>
    any(const T& x)
        : policy(anyimpl::get_policy<anyimpl::empty_any>()), object(NULL)
    {
        assign(x);
    }

    /// Empty constructor.
    any()
        : policy(anyimpl::get_policy<anyimpl::empty_any>()), object(NULL)
    { }

    /// Special initializing constructor for string literals.
    any(const char* x)
        : policy(anyimpl::get_policy<anyimpl::empty_any>()), object(NULL)
    {
        assign(x);
    }

    /// Copy constructor.
    any(const any& x)
        : policy(anyimpl::get_policy<anyimpl::empty_any>()), object(NULL)
    {
        assign(x);
    }

    /// Destructor.
    ~any() {
        policy->static_delete(&object);
    }

    /// Assignment function from another any.
    any& assign(const any& x) {
        reset();
        policy = x.policy;
        policy->clone(&x.object, &object);
        return *this;
    }

    /// Assignment function.
    template <typename T>
    any& assign(const T& x) {
        reset();
        policy = anyimpl::get_policy<T>();
        policy->copy_from_value(&x, &object);
        return *this;
    }

    /// Assignment operator.
    template<typename T>
    any& operator=(const T& x) {
        return assign(x);
    }

    /// Assignment operator, specialed for literal strings.
    /// They have types like const char [6] which don't work as expected.
    any& operator=(const char* x) {
        return assign(x);
    }

    /// Utility functions
    any& swap(any& x) {
        std::swap(policy, x.policy);
        std::swap(object, x.object);
        return *this;
    }

    /// Cast operator. You can only cast to the original type.
    template<typename T>
    T& cast() {
        if (policy != anyimpl::get_policy<T>())
            throw anyimpl::bad_any_cast();
        T* r = reinterpret_cast<T*>(policy->get_value(&object));
        return *r;
    }

    /// Returns true if the any contains no value.
    bool empty() const {
        return policy == anyimpl::get_policy<anyimpl::empty_any>();
    }

    /// Frees any allocated memory, and sets the value to NULL.
    void reset() {
        policy->static_delete(&object);
        policy = anyimpl::get_policy<anyimpl::empty_any>();
    }

    /// Returns true if the two types are the same.
    bool compatible(const any& x) const {
        return policy == x.policy;
    }
};
}



class A
{
public:
    A()
    {
        qDebug("A");
    }
    ~A()
    {
        qDebug("~A");
    }
};

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
        qDebug()<<"call queueInLoop";
        uv_mutex_lock(&m_functor_mutex);
        m_functors.push(functor);
        uv_mutex_unlock(&m_functor_mutex);
        uv_async_send(m_functor_async.get());
        qDebug()<<"after queueInLoop";
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

class Connection:public Tcp
{
public:
    Connection(Loop *loop):
        Tcp(loop)
    {
        static int id = 0;
        id++;
        m_id = id;
        qDebug()<<"Connection"<<m_id;
    }

    ~Connection()
    {
        qDebug()<<"~Connection"<<m_id;
    }

    int id()
    {
        return m_id;
    }

private:
    int m_id;
};
typedef shared_ptr<Connection> ConnectionPtr;


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

    void setMessageCallback(const ReadCallback &cb)
    {
        m_readCallback = cb;
    }

private:
    LoopEx* m_loop;
    Tcp m_tcpServer;
    std::unordered_map<int, ConnectionPtr> m_connections;
    ReadCallback m_readCallback;

    void onNewConnection(int status)
    {
        ConnectionPtr connection(new Connection(m_loop));
        m_connections.insert(std::make_pair(connection->id(), connection));
        //Tcp* connection = new Tcp(m_loop);
        m_tcpServer.accept(connection.get());

        connection->setCloseCallback(std::bind(&TcpServer::onConnectionClose, this, connection));
        connection->read_start(m_readCallback);
        qDebug()<<"onNewConnection END";
    }

    void onConnectionClose(const ConnectionPtr &connection)
    {
        qDebug()<<"#1";
        m_loop->queueInLoop([connection]()
        {
            qDebug()<<"Run in loop";
        });
        qDebug()<<"#2";

        qDebug()<<"onConnectionClose Chang#1";
        m_connections.erase(connection->id());
        connection->setCloseCallback(nullptr);
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

    LoopEx loop;
    TcpServer server(&loop);
    server.listen("0.0.0.0", 80);
    server.setMessageCallback([](QByteArray d)
    {
        qDebug()<<"recv";
        qDebug()<<d;
    });
    qDebug()<<"begin run";
    loop.run();

    qDebug()<<"OVER";
    return a.exec();
}
