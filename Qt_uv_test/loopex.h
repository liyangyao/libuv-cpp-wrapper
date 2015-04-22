/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/22

****************************************************************************/

#ifndef LOOPEX_H
#define LOOPEX_H

#include "uvpp.h"
#include "loop.h"
#include "idle.h"
#include "async.h"

namespace uv{
class LoopEx:public Loop
{
public:
    explicit LoopEx():
        m_idle(this),
        m_async(this)
    {
    }

    void queueInLoop(const Functor &functor)
    {
        m_functors.push_back(functor);
        m_async.send([this]()
        {
            if (!m_functors.empty())
            {
                LoopEx* _this = this;
                m_idle.start([_this]()
                {
                    if (m_functors.empty())
                    {
                        m_idle.stop();
                    }
                    else{
                        Functor& f = m_functors.back();
                        f();
                        m_functors.pop_back();
                    }
                });
            }
            else{
                m_idle.stop();
            }
        });
    }

private:
    std::vector<Functor> m_functors;
    Idle m_idle;
    Async m_async;
};
}

#endif // LOOPEX_H
