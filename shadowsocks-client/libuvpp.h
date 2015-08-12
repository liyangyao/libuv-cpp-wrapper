/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/20

****************************************************************************/

#ifndef LIBUVPP_H
#define LIBUVPP_H

#include <memory>
#include <functional>
#include <QObject>
#include <libuv/include/uv.h>
#include <QDebug>

namespace uv{
#define DISABLE_COPY(Class) \
    Class(const Class &); \
    Class &operator=(const Class &);

using std::shared_ptr;
using std::unique_ptr;
using std::function;

typedef function<void()> Callback;
typedef function<void(bool connected)> ConnectCallback;
typedef function<void(const QByteArray &data)> MessageCallback;
typedef function<void(int status)> RequestCallback;
typedef function<void(const QByteArray &data)> DataCallback;

class Loop
{
public:
    explicit Loop()
    {
        uv_loop_init(&m_uv_loop);
    }

    void run()
    {
        uv_run(&m_uv_loop, UV_RUN_DEFAULT);
    }

    void close()
    {
        uv_loop_close(&m_uv_loop);
    }

    int alive()
    {
        return uv_loop_alive(&m_uv_loop);
    }

    void stop()
    {
        uv_stop(&m_uv_loop);
    }

    uint64_t now()
    {
        return uv_now(&m_uv_loop);
    }

    void update_time()
    {
        uv_update_time(&m_uv_loop);
    }

    uv_loop_t *handle()
    {
        return &m_uv_loop;
    }

private:
    uv_loop_t m_uv_loop;
    DISABLE_COPY(Loop)
};

template <typename HANDLE_T>
class Handle
{
public:
    explicit Handle()
    {
        m_uv_handle = new HANDLE_T;
    }

    virtual ~Handle()
    {
        close();
    }

    uv_handle_t* uv_handle()
    {
        return reinterpret_cast<uv_handle_t *>(m_uv_handle);
    }

    HANDLE_T *handle()
    {
        return m_uv_handle;
    }

    template <typename T>
    T *handle()
    {
        return reinterpret_cast<T *>(m_uv_handle);
    }

    bool is_active()
    {
        return uv_is_active(uv_handle());
    }

    bool is_closeing()
    {
        return uv_is_closing(uv_handle());
    }

    void ref()
    {
        uv_ref(uv_handle());
    }

    void unref()
    {
        uv_unref(uv_handle());
    }

    bool has_ref()
    {
        uv_has_ref(uv_handle());
    }

private:
    HANDLE_T *m_uv_handle;

    void close()
    {
        uv_close(uv_handle(), close_cb);
    }

    static void close_cb(uv_handle_t* handle)
    {
        HANDLE_T* _handle = reinterpret_cast<HANDLE_T *>(handle);
        delete _handle;
    }
    DISABLE_COPY(Handle)
};

struct WriteRequest
{
public:
    explicit WriteRequest(const QByteArray &ba, const RequestCallback &cb):
         m_byteArray(ba),
         m_writeCallback(cb)
    {
        m_buf.base = m_byteArray.data();
        m_buf.len = m_byteArray.length();
        m_req.data = this;
    }

    uv_write_t *get()
    {
        return &m_req;
    }

    const uv_buf_t *buf()
    {
        return &m_buf;
    }

    const RequestCallback& writeCallback()
    {
        return m_writeCallback;
    }

private:
    uv_write_t m_req;
    uv_buf_t m_buf;
    QByteArray m_byteArray;
    RequestCallback m_writeCallback;
    DISABLE_COPY(WriteRequest)
};

template <typename HANDLE_T>
class Stream: public Handle<HANDLE_T>
{
public:
    explicit Stream():
        Handle<HANDLE_T>()
    {
        handle()->data = this;
    }

    bool accept(Stream *client)
    {
        return uv_accept(handle<uv_stream_t>(), client->handle<uv_stream_t>())>=0;
    }

    int read_start()
    {
        return uv_read_start(handle<uv_stream_t>(), on_alloc_cb, on_read_cb);
    }

    int read_stop()
    {
        return uv_read_stop(handle<uv_stream_t>());
    }

    int write(const QByteArray &ba, const RequestCallback &cb)
    {
        WriteRequest *wr = new WriteRequest(ba, cb);
        return uv_write(wr->get(), handle<uv_stream_t>(), wr->buf(), 1, on_write_cb);
    }

    int shutdown()
    {
        uv_shutdown_t* req = new uv_shutdown_t;
        return uv_shutdown(req, handle<uv_stream_t>(), on_shutdown_cb);
    }

    bool listen()
    {
        return uv_listen(handle<uv_stream_t>(), 128, on_connection_cb)>=0;
    }

    void onConnection(const Callback &cb)
    {
        m_onConnection = cb;
    }

    void onClose(const Callback &cb)
    {
        m_onClose = cb;
    }

    void onMessage(const MessageCallback &cb)
    {
        m_onMessage = cb;
    }

private:    
    Callback m_onConnection;
    Callback m_onClose;
    MessageCallback m_onMessage;

    static void on_write_cb(uv_write_t* req, int status)
    {
        WriteRequest *wr = reinterpret_cast<WriteRequest *>(req->data);
        if (wr->writeCallback())
        {
            wr->writeCallback()(status);
        }
        delete wr;
    }

    static void on_alloc_cb(uv_handle_t* /*handle*/, size_t /*suggested_size*/, uv_buf_t* buf)
    {
        static char BUFFER[65535];
        buf->base = BUFFER;
        buf->len = 65535;
    }

    static void on_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
    {
        //nread is > 0 if there is data available, 0 if libuv is done reading for now, or < 0 on error.
        Stream *_this = reinterpret_cast<Stream *>(stream->data);
        if (nread>0)
        {
            QByteArray ba(buf->base, nread);
            if (_this->m_onMessage)
            {
                _this->m_onMessage(ba);
            }
        }
        else if (nread<0)
        {
            _this->read_stop();
            if (_this->m_onClose)
            {
                _this->m_onClose();
            }
        }
    }

    static void on_shutdown_cb(uv_shutdown_t* req, int /*status*/)
    {
        delete req;
    }

    static void on_connection_cb(uv_stream_t* server, int /*status*/)
    {
        Stream *_this = reinterpret_cast<Stream *>(server->data);        
        if (_this->m_onConnection)
        {
            _this->m_onConnection();
        }
    }
    DISABLE_COPY(Stream)
};


class Tcp: public Stream<uv_tcp_t>
{
public:
    explicit Tcp(Loop *loop):
        Stream<uv_tcp_t>()
    {
        //qDebug()<<"Tcp";
        uv_tcp_init(loop->handle(), handle());
    }

    ~Tcp()
    {
        //qDebug()<<"~Tcp";
    }

    int nodelay(int enable)
    {
        return uv_tcp_nodelay(handle(), enable);
    }

    int keepalive(int enable, unsigned int delay)
    {
        return uv_tcp_keepalive(handle(), enable, delay);
    }

    int connect(const char *ip, int port)
    {
        struct sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        if (!m_connect_req)
        {
            m_connect_req.reset(new uv_connect_t);
            m_connect_req->data = this;
        }
        return uv_tcp_connect(m_connect_req.get(), handle(),
                              reinterpret_cast<const struct sockaddr *>(&addr),
                              on_connect_cb);
    }

    int bind(const char *ip, int port)
    {
        struct sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        return uv_tcp_bind(handle(), reinterpret_cast<const struct sockaddr *>
                           (&addr), 0);
    }

    void onConnect(const ConnectCallback &cb)
    {
        m_onConnect = cb;
    }

private:
    unique_ptr<uv_connect_t> m_connect_req;
    ConnectCallback m_onConnect;

    static void on_connect_cb(uv_connect_t* req, int status)
    {
        bool connected = status==0;
        Tcp *_this = reinterpret_cast<Tcp *>(req->data);
        if (_this->m_onConnect)
        {
            _this->m_onConnect(connected);
        }
        if (connected)
        {
            _this->read_start();
        }
    }
    DISABLE_COPY(Tcp)
};

class Timer: public Handle<uv_timer_t>
{
public:
    Timer(Loop* loop):
        Handle<uv_timer_t>()
    {
        uv_timer_init(loop->handle(), handle());
    }

    void start(const Callback& functor, uint64_t timeout, uint64_t repeat)
    {
        m_functor = functor;
        uv_timer_start(handle(), timer_cb, timeout, repeat);
    }

    void stop()
    {
        m_functor = nullptr;
        uv_timer_stop(handle());
    }

    void again()
    {
        uv_timer_again(handle());
    }

    void setRepeat(uint64_t repeat)
    {
        uv_timer_set_repeat(handle(), repeat);
    }

    uint64_t getRepeat()
    {
        return uv_timer_get_repeat(handle());
    }

private:
    Callback m_functor;
    static void timer_cb(uv_timer_t* handle)
    {
        Timer* _this = (Timer *)handle->data;
        if (_this->m_functor)
        {
            _this->m_functor();
        }
    }
};

class Thread: public Handle<uv_thread_t>
{
public:
    explicit Thread():
        Handle<uv_thread_t>()
    {

    }

    int create(const Callback& functor)
    {
        m_functor = functor;
        return uv_thread_create(handle(), entry, this);
    }

    int join()
    {
        return uv_thread_join(handle());
    }

private:
    Callback m_functor;
    static void entry(void* arg)
    {
        Thread* _this = (Thread *)arg;
        if (_this->m_functor)
        {
            _this->m_functor();
        }
    }
    DISABLE_COPY(Thread);
};

class Mutex
{
public:
    explicit Mutex()
    {
        uv_mutex_init(&m_mutex);
    }
    ~Mutex()
    {
        uv_mutex_destroy(&m_mutex);
    }

    void lock()
    {
        uv_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        uv_mutex_unlock(&m_mutex);
    }

    int tryLock()
    {
        return uv_mutex_trylock(&m_mutex);
    }

    uv_mutex_t* handle()
    {
        return &m_mutex;
    }

private:
    uv_mutex_t m_mutex;
    DISABLE_COPY(Mutex);
};

class MutexLocker
{
public:
    explicit MutexLocker(Mutex* mutex):
        m_mutex(mutex)
    {
        m_mutex->lock();
    }

    ~MutexLocker()
    {
        m_mutex->unlock();
    }

private:
    Mutex* m_mutex;
    DISABLE_COPY(MutexLocker);
};

class Async: public Handle<uv_async_t>
{
public:
    explicit Async(Loop* loop,const Callback& functor):
        Handle<uv_async_t>(),
        m_functor(functor)
    {
        uv_async_init(loop->handle(), handle(), async_cb);
        handle()->data = this;
    }

    void send()
    {
        uv_async_send(handle());
    }

private:
    Callback m_functor;
    static void async_cb(uv_async_t* handle)
    {
        Async* _this = (Async *)handle->data;
        if (_this->m_functor)
        {
            _this->m_functor();
        }
    }
    DISABLE_COPY(Async);
};
}
#endif // LIBUVPP_H
