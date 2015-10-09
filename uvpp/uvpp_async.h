#ifndef UVPP_ASYNC_H
#define UVPP_ASYNC_H

#include "uvpp_define.h"
#include "uvpp_handle.h"

namespace uv{

class Async: public Handle<uv_async_t>
{
public:
    explicit Async(const Callback& functor, Loop* loop):
        Handle<uv_async_t>(),
        m_functor(functor)
    {
        uv_async_init(loop->handle(), handle(), async_cb);
    }

    explicit Async(const Callback& functor, uv_loop_t* loop = uv_default_loop()):
        Handle<uv_async_t>(),
        m_functor(functor)
    {
        uv_async_init(loop, handle(), async_cb);
    }

    void send()
    {
        uv_async_send(handle());
    }

private:
    Callback m_functor;
    static void async_cb(uv_async_t* handle)
    {
        Async* self = (Async *)handle->data;
        if (self->m_functor)
        {
            self->m_functor();
        }
    }
    DISABLE_COPY(Async);
};
}

#endif // UVPP_ASYNC_H
