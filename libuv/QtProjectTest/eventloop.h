#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <uv.h>
#include <functional>
#include <vector>
class EventLoop
{
public:
    explicit EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    uv_loop_t *handle() { return &m_handle;}
    typedef std::function<void()> Functor;
    void queueInLoop(const Functor&);

private:
    uv_loop_t m_handle;
    uv_idle_t m_idle;
    std::vector<Functor> m_pendingFunctors;
    static void idle_cb(uv_idle_t* handle);
};

#endif // EVENTLOOP_H
