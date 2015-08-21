/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/18

****************************************************************************/

#ifndef UVPP_IDLE_H
#define UVPP_IDLE_H

#include "uvpp_define.h"
#include "uvpp_handle.h"
#include "uvpp_loop.h"

namespace uvpp{

class Idle: public Handle<uv_idle_t>
{
public:
    explicit Idle(Loop* loop, const Functor &functor):
        Handle<uv_idle_t>(),
        m_functor(functor)
    {
        uv_idle_init(loop->handle(), handle());
    }

    void start()
    {
        uv_idle_start(handle(), idle_cb);
    }

    void stop()
    {
        uv_idle_stop(handle());
    }

private:
    Functor m_functor;
    static void idle_cb(uv_idle_t* handle)
    {
        Idle* _this = (Idle *)handle->data;
        if (_this->m_functor)
        {
            _this->m_functor();
        }
    }
    DISABLE_COPY(Idle);
};
}

#endif // UVPP_IDLE_H
