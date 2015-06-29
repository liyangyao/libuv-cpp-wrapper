/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/28

****************************************************************************/

#ifndef LOOPEX_H
#define LOOPEX_H
#include <queue>
#include "loop.h"
#include "handle.h"

namespace uv {
class LoopEx:public Loop
{
public:
    LoopEx():
        Loop()
    {
        uv_mutex_init(&m_functor_mutex);
        m_functor_async.get()->data = this;
        uv_async_init(handle(), m_functor_async.get(), functor_async_cb);
    }

    void queueInLoop(const Functor &functor)
    {
        uv_mutex_lock(&m_functor_mutex);
        m_functors.push(functor);
        uv_mutex_unlock(&m_functor_mutex);
        uv_async_send(m_functor_async.get());
        qDebug()<<"after queueInLoop";
    }


private:
    std::queue<Functor> m_functors;
    Handle<uv_async_t> m_functor_async;
    uv_mutex_t m_functor_mutex;
    void onFunctor()
    {
        while (true)
        {
            Functor fun = nullptr;
            uv_mutex_lock(&m_functor_mutex);
            if (!m_functors.empty())
            {
                 fun.swap(m_functors.front());
                 m_functors.pop();
            }
            uv_mutex_unlock(&m_functor_mutex);
            if (fun)
            {
                fun();
            }
            else{
                break;
            }
        }
    }

    static void functor_async_cb(uv_async_t* handle)
    {
        qDebug()<<"functor_async_cb";
        LoopEx *_this = reinterpret_cast<LoopEx *>(handle->data);
        _this->onFunctor();
    }
};

}

#endif // LOOPEX_H
