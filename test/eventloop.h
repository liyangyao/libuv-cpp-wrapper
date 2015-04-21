/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/19

****************************************************************************/

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <libuv/include/uv.h>
#include <QObject>

namespace libuv{

class Loop
{
public:
    explicit loop()
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
    Q_DISABLE_COPY(EventLoop)
};
}

#endif // EVENTLOOP_H
