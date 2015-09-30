/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/18

****************************************************************************/

#ifndef UVPP_ASYNCFUNCTOR_H
#define UVPP_ASYNCFUNCTOR_H

#include <deque>
#include <memory>
#include <uvpp/uvpp_async.h>
#include <uvpp/uvpp_thread.h>

namespace uv{

class AsyncFunctor
{
public:
    explicit AsyncFunctor(Loop* loop)
    {
        m_async.reset(new Async(loop, std::bind(&AsyncFunctor::onFunctor, this)));
    }

    void queue(const Callback &functor)
    {
        MutexLocker lock(&m_mutex);
        m_functors.push_back(functor);
        m_async->send();
    }

private:
    std::deque<Callback> m_functors;
    std::unique_ptr<Async> m_async;
    Mutex m_mutex;
    void onFunctor()
    {
        while (true)
        {
            Callback fun = nullptr;
            {
                MutexLocker lock(&m_mutex);
                if (!m_functors.empty())
                {
                    fun.swap(m_functors.front());
                    m_functors.pop_front();
                }
            }

            if (fun)
            {
                fun();
            }
            else{
                break;
            }
        }
    }
    DISABLE_COPY(AsyncFunctor)
};
}
#endif // UVPP_ASYNCFUNCTOR_H
