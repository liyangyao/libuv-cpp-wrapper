/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/19

****************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#include <libuv/include/uv.h>
#include <functional>
#include "eventloop.h"

namespace libuv{
class Timer
{
public:
    typedef std::function<void()> Callback;
    Timer(EventLoop *eventLoop)
    {
        uv_timer_init(eventLoop->loop(), &m_handle);
        m_handle.data = this;
    }

    void start(const Callback &cb, uint64_t timeout, uint64_t repeat)
    {
        m_callback = cb;
        uv_timer_start(&m_handle, timer_cb, timeout, repeat);
    }

    void stop()
    {
        uv_timer_stop(&m_handle);
    }

private:
    uv_timer_t m_handle;
    Callback m_callback;
    static void timer_cb(uv_timer_t* handle)
    {
        Timer* _this = reinterpret_cast<Timer *>(handle->data);
        if (_this->m_callback)
        {
            _this->m_callback;
        }
    }
};
}

#endif // TIMER_H
