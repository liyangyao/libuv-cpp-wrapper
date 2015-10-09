/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#ifndef UVPP_LOOP_H
#define UVPP_LOOP_H

#include "uvpp_define.h"

namespace uv{

class Loop
{
public:
    explicit Loop(bool defaultLoop = true):
        m_uvLoop(defaultLoop?uv_default_loop():new uv_loop_t)
    {
        if (!defaultLoop)
        {
            uv_loop_init(m_uvLoop.get());
        }
        m_uvLoop->data = this;
        buffer.in_use = false;
    }

    //Run the event loop.
    bool run()
    {
        return uv_run(m_uvLoop.get(), UV_RUN_DEFAULT)==0;
    }

    //Stop running the event loop.
    void stop()
    {
        uv_stop(m_uvLoop.get());
    }

    //Return event loop time, expressed in nanoseconds.
    uint64_t now()
    {
        return uv_now(m_uvLoop.get());
    }

    //Update event loop's notion of time by querying the kernel.
    void update_time()
    {
        uv_update_time(m_uvLoop.get());
    }

    //Get the poll timeout, or -1 for no timeout.
    int timeout()
    {
        return uv_backend_timeout(m_uvLoop.get());
    }

    struct loop_work_ctx
    {
        uv_work_t work;
        Callback work_cb;
        CallbackWithResult after_cb;
    };

    void queue_work(const Callback &work_cb, const CallbackWithResult &after_cb)
    {
        loop_work_ctx* req = new loop_work_ctx;
        req->work_cb = work_cb;
        req->after_cb = after_cb;
        uv_queue_work(m_uvLoop.get(), (uv_work_t *)req, loop_work_cb, loop_after_work_cb);
    }

    bool run_once()
    {        
        return uv_run(m_uvLoop.get(), UV_RUN_ONCE)==0;
    }


    void close()
    {
        uv_loop_close(m_uvLoop.get());
    }

    bool alive()
    {
        return uv_loop_alive(m_uvLoop.get())!=0;
    }

    uv_loop_t *handle()
    {
        return m_uvLoop.get();
    }

    #define SLAB_SIZE 65536
    struct{
        char slab[SLAB_SIZE];
        bool in_use;
    } buffer;
private:
    static void loop_work_cb(uv_work_t* req)
    {
        loop_work_ctx *ctx = (loop_work_ctx *)req;
        if (ctx->work_cb)
        {
            ctx->work_cb();
        }
    }

    static void loop_after_work_cb(uv_work_t* req, int status)
    {
        loop_work_ctx *ctx = (loop_work_ctx *)req;
        if (ctx->after_cb)
        {
            ctx->after_cb(status);
        }
        delete ctx;
    }

    std::unique_ptr<uv_loop_t> m_uvLoop;
    DISABLE_COPY(Loop)
};
}

#endif // UVPP_LOOP_H
