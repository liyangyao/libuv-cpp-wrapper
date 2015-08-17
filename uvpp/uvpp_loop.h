/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#ifndef UVPP_LOOP_H
#define UVPP_LOOP_H

#include "uvpp_define.h"

namespace uvpp{

class Loop
{
public:
    explicit Loop()
    {
        uv_loop_init(&m_uv_loop);
    }

    bool run()
    {
        return uv_run(&m_uv_loop, UV_RUN_DEFAULT)==0;
    }

    bool run_once()
    {
        return uv_run(&m_uv_loop, UV_RUN_ONCE)==0;
    }


    void close()
    {
        uv_loop_close(&m_uv_loop);
    }

    bool alive()
    {
        return uv_loop_alive(&m_uv_loop)!=0;
    }

    void stop()
    {
        uv_stop(&m_uv_loop);
    }

    uint64_t now()
    {
        return uv_now(&m_uv_loop);
    }

    void update_time()
    {
        uv_update_time(&m_uv_loop);
    }

    uv_loop_t *handle()
    {
        return &m_uv_loop;
    }

private:
    uv_loop_t m_uv_loop;
    DISABLE_COPY(Loop)
};
}

#endif // UVPP_LOOP_H
