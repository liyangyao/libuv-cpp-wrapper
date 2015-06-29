/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#include "uvpp.h"
#include "loop.h"
#include "handle.h"

namespace uv{

class Timer:public Handle<uv_timer_t>
{
public:
    explicit Timer(Loop *loop):
        Handle<uv_timer_t>()
    {
        uv_timer_init(loop->handle(), get());
    }

    void start(const Callback& cb, uint64_t timeout, uint64_t repeat)
    {
        m_callback = cb;
        uv_timer_start(get(), on_timer_cb, timeout, repeat);
    }

    void stop()
    {
        uv_timer_stop(get());
        m_callback = nullptr;
    }

private:
    Callback m_callback;
    static void on_timer_cb(uv_timer_t* handle)
    {
        Timer *_this = reinterpret_cast<Timer *>(handle->data);
        if (_this->m_callback)
        {
            _this->m_callback();
        }
    }
    DISABLE_COPY(Timer)
};
}

#endif // TIMER_H
