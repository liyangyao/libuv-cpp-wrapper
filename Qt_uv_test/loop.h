/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef LOOP_H
#define LOOP_H
#include "uvqt.h"

namespace uv {
class Loop
{
public:
    explicit Loop()
    {
         uv_loop_init(&m_loop);
    }
    void run()
    {
        uv_run(&m_loop, UV_RUN_DEFAULT);
    }
    void close()
    {
        uv_loop_close(&m_loop);
    }

    uv_loop_t *handle()
    {
        return &m_loop;
    }

private:
    uv_loop_t m_loop;
    Q_DISABLE_COPY(Loop)
};
}
#endif // LOOP_H
