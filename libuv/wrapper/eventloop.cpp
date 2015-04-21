#include "eventloop.h"

namespace LibuvWrapper{

void LibuvWrapper::EventLoop::exec()
{
    uv_run(&m_loop, UV_RUN_DEFAULT);
}

void LibuvWrapper::EventLoop::quit()
{
    uv_loop_close(&m_loop);
}

}
