/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/11

****************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#include "uvpp.h"
#include "loop.h"
namespace uv{

class Timer: public Handle<uv_timer_t>
{
public:
    Timer(Loop* loop):
        Handle<uv_timer_t>()
    {
        uv_timer_init(loop->handle(), get());
    }

    void start(const Functor& functor, uint64_t timeout, uint64_t repeat)
    {
        m_functor = functor;
        uv_timer_start(get(), timer_cb, timeout, repeat);
    }

    void stop()
    {
        m_functor = nullptr;
        uv_timer_stop(get());
    }

    void again()
    {
        uv_timer_again(get());
    }

    void setRepeat(uint64_t repeat)
    {
        uv_timer_set_repeat(get(), repeat);
    }

    uint64_t getRepeat()
    {
        return uv_timer_get_repeat(get());
    }

private:
    Functor m_functor;
    static void timer_cb(uv_timer_t* handle)
    {
        Timer* _this = (Timer *)handle->data;
        if (_this->m_functor)
        {
            _this->m_functor();
        }
    }
};
}

#endif // TIMER_H
