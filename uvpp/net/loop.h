/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef LOOP_H
#define LOOP_H
#include "uvpp.h"
#include "handle.h"

namespace uv {

class Loop
{
public:
    explicit Loop()
    {
         uv_loop_init(&m_uv_loop);
    }

    void run()
    {
        uv_run(&m_uv_loop, UV_RUN_DEFAULT);
    }

    void close()
    {
        uv_loop_close(&m_uv_loop);
    }

    int alive()
    {
        return uv_loop_alive(&m_uv_loop);
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
#endif // LOOP_H
