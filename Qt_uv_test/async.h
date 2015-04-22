/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/21

****************************************************************************/

#ifndef ASYNC_H
#define ASYNC_H

#include "uvpp.h"
#include "loop.h"
#include "handle.h"

namespace uv{

class Async:public Handle<uv_async_t>
{
public:
    explicit Async(Loop *loop):
        Handle<uv_async_t>()
    {
        uv_async_init(loop->handle(), get(), on_async_cb);
    }

    int send(const Callback& cb)
    {
        m_callback = cb;
        return uv_async_send(get());
    }


private:
    Callback m_callback;
    static void on_async_cb(uv_async_t* handle)
    {
        Async *_this = reinterpret_cast<Async *>(handle->data);
        if (_this->m_callback)
        {
            _this->m_callback();
        }
    }
    DISABLE_COPY(Async)
};
}

#endif // ASYNC_H
