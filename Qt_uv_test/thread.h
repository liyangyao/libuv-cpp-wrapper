/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/22

****************************************************************************/

#ifndef THREAD_H
#define THREAD_H

#include "uvpp.h"

namespace uv {

struct ThreadData
{
    Functor functor;
    uv_thread_t id;
};

class Thread
{
public:
    explicit Thread(const Functor &functor)
    {
        m_data = new ThreadData;
        m_data->functor = functor;
        uv_thread_create(&m_data->id, on_thread_cb, m_data);
    }
    int join()
    {
        return uv_thread_join(&m_data->id);
    }

private:
    ThreadData* m_data;
    static void on_thread_cb(void *arg)
    {
        ThreadData* data = reinterpret_cast<ThreadData *>(arg);
        if (data->functor)
        {
            data->functor();
        }
        delete data;
    }

    DISABLE_COPY(Thread)
};

}

#endif // THREAD_H
