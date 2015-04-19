#include "eventloop.h"

EventLoop::EventLoop()
{
    uv_loop_init(&m_handle);
    uv_idle_init(&m_handle, &m_idle);
    m_idle.data = this;
}

EventLoop::~EventLoop()
{
    uv_loop_close(&m_handle);
}

void EventLoop::loop()
{
    uv_run(&m_handle, UV_RUN_DEFAULT);
}

void EventLoop::quit()
{
    uv_stop(&m_handle);
}

void EventLoop::idle_cb(uv_idle_t* handle)
{
    EventLoop *_this = (EventLoop *)handle->data;
    for(auto it=_this->m_pendingFunctors.begin(); it<_this->m_pendingFunctors.end(); it++)
    {
        Functor& f = *it;
        f();
    }
    _this->m_pendingFunctors.clear();
    uv_idle_stop(handle);
}

void EventLoop::queueInLoop(const EventLoop::Functor &cb)
{
    m_pendingFunctors.push_back(cb);
    uv_idle_start(&m_idle, idle_cb);
}
