#ifndef UVPP_TIMER_H
#define UVPP_TIMER_H

#include "uvpp_define.h"
#include "uvpp_handle.h"

namespace uvpp{
class Timer: public Handle<uv_timer_t>
{
public:
    Timer(Loop* loop):
        Handle<uv_timer_t>()
    {
        handle()->data = this;
        uv_timer_init(loop->handle(), handle());
    }

    //Start the Timer.
    void start(const TimerCallback& functor, uint64_t timeout, uint64_t repeat)
    {
        m_functor = functor;
        uv_timer_start(handle(), timer_cb, timeout, repeat);
    }

    //Stop the Timer.
    void stop()
    {
        m_functor = nullptr;
        uv_timer_stop(handle());
    }

    //stop the timer, and if it is repeating restart it using the repeat value as the timeout.
    void again()
    {
        uv_timer_again(handle());
    }

    void setRepeat(uint64_t repeat)
    {
        uv_timer_set_repeat(handle(), repeat);
    }

    uint64_t getRepeat()
    {
        return uv_timer_get_repeat(handle());
    }

private:
    TimerCallback m_functor;
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

#endif // UVPP_TIMER_H
