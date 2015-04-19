/****************************************************************************

Creator: liyangyao@gmail.com
Date: 4/19/2015

****************************************************************************/

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <libuv/include/uv.h>
#include <QObject>

namespace LibuvWrapper{

class EventLoop
{
public:
    explicit EventLoop();
    ~EventLoop();

    void exec()
    {
        uv_run(&m_loop, UV_RUN_DEFAULT);
    }

    void quit()
    {
        uv_loop_close(&m_loop);
    }

private:
    uv_loop_t m_loop;
    Q_DISABLE_COPY(EventLoop)
};
}

#endif // EVENTLOOP_H

