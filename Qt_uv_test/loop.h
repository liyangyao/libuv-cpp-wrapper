/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef LOOP_H
#define LOOP_H
#include "uvpp.h"
#include "handle.h"

namespace uv {

struct uvpp_async_t
{
    uv_async_t handle;
    Functor functor;
};

class Loop
{
public:
    explicit Loop()
    {
         uv_loop_init(&m_loop);
    }
    void run()
    {
        uv_run(&m_loop, UV_RUN_DEFAULT);
    }
    void close()
    {
        uv_loop_close(&m_loop);
    }

    int alive()
    {
        return uv_loop_alive(&m_loop);
    }

    void stop()
    {
        uv_stop(&m_loop);
    }

    uint64_t now()
    {
        return uv_now(&m_loop);
    }

    void update_time()
    {
        uv_update_time(&m_loop);
    }

    uv_loop_t *handle()
    {
        return &m_loop;
    }

    void queueInLoop(const Functor &functor)
    {
        Handle<uvpp_async_t> *async = new Handle<uvpp_async_t>;
        async->get()->functor = functor;
        uv_async_init(&m_loop, &async->get()->handle, on_async_cb);
        uv_async_send(&async->get()->handle);
    }

private:
    uv_loop_t m_loop;
    static void on_async_cb(uv_async_t* handle)
    {
        Handle<uvpp_async_t> *async = reinterpret_cast<Handle<uvpp_async_t> *>
                (handle->data);
        if (async->get()->functor)
        {
            async->get()->functor();
        }
        delete async;
    }
    DISABLE_COPY(Loop)
};
}
#endif // LOOP_H
