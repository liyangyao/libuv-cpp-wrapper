#ifndef UVPP_ASYNC_H
#define UVPP_ASYNC_H

#include "uvpp_define.h"
#include "uvpp_handle.h"

namespace uv{

class Async: public Handle<uv_async_t>
{
public:
    explicit Async(Loop* loop,const Callback& functor):
        Handle<uv_async_t>(),
        m_functor(functor)
    {
        uv_async_init(loop->handle(), handle(), async_cb);
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

#endif // UVPP_ASYNC_H
