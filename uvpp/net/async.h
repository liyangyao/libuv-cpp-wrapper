/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/11

****************************************************************************/

#ifndef ASYNC_H
#define ASYNC_H

#include "uvpp.h"
#include "loop.h"
namespace uv{

class Async: public Handle<uv_async_t>
{
public:
    Async(Loop* loop,const Functor& functor):
        Handle<uv_async_t>(),
        m_functor(functor)
    {
        uv_async_init(loop->handle(), get(), async_cb);
    }

    void send()
    {
        uv_async_send(get());
    }

private:
    Functor m_functor;
    static void async_cb(uv_async_t* handle)
    {
        Async* _this = (Async *)handle->data;
        if (_this->m_functor)
        {
            _this->m_functor();
        }
    }
};
}

#endif // ASYNC_H
