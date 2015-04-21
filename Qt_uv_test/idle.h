/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef IDLE_H
#define IDLE_H

#include "uvqt.h"
#include "loop.h"
#include "handle.h"

namespace uv{

class Idle:public Handle<uv_idle_t>
{
public:
    explicit Idle(Loop *loop):
        Handle<uv_idle_t>()
    {
        uv_idle_init(loop->handle(), ptr());
    }

    void start(const Callback& cb)
    {
        m_callback = cb;
        uv_idle_start(ptr(), on_idle_cb);
    }

    void stop()
    {
        uv_idle_stop(ptr());
        m_callback = nullptr;
    }

private:
    Callback m_callback;
    static void on_idle_cb(uv_idle_t* handle)
    {
        Idle *_this = reinterpret_cast<Idle *>(handle->data);
        if (_this->m_callback)
        {
            _this->m_callback();
        }
    }
};
}

#endif // IDLE_H