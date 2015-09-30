#ifndef UVPP_THREAD_H
#define UVPP_THREAD_H

#include "uvpp_define.h"
#include "uvpp_handle.h"

namespace uv{

class Mutex
{
public:
    explicit Mutex()
    {
        uv_mutex_init(&m_mutex);
    }
    ~Mutex()
    {
        uv_mutex_destroy(&m_mutex);
    }

    //Lock the mutex object
    void lock()
    {
        uv_mutex_lock(&m_mutex);
    }

    //Unlock the mutex object
    void unlock()
    {
        uv_mutex_unlock(&m_mutex);
    }

    //Try to lock the mutex, return True if the lock was acquired, False otherwise
    int tryLock()
    {
        return uv_mutex_trylock(&m_mutex);
    }

    uv_mutex_t* handle()
    {
        return &m_mutex;
    }

private:
    uv_mutex_t m_mutex;
    DISABLE_COPY(Mutex);
};

class MutexLocker
{
public:
    explicit MutexLocker(Mutex* mutex):
        m_mutex(mutex)
    {
        m_mutex->lock();
    }

    ~MutexLocker()
    {
        m_mutex->unlock();
    }

private:
    Mutex* m_mutex;
    DISABLE_COPY(MutexLocker);
};

class Condition
{
public:
    explicit Condition()
    {
        uv_cond_init(&m_cond);
    }

    ~Condition()
    {
        uv_cond_destroy(&m_cond);
    }

    //Unblock at least one of the threads that are blocked on this condition
    void signal()
    {
        uv_cond_signal(&m_cond);
    }

    //Unblock all threads currently blocked on this condition
    void broadcast()
    {
        uv_cond_broadcast(&m_cond);
    }

    //Block on this condition variable, mutex needs to be locked
    void wait(Mutex *mutex)
    {
        uv_cond_wait(&m_cond, mutex->handle());
    }

    //Block on this condition variable for the given amount of time, mutex needs to be locked
    int timedwait(Mutex *mutex, uint64_t timeout)
    {
       return uv_cond_timedwait(&m_cond, mutex->handle(), timeout);
    }

private:
    uv_cond_t m_cond;
};

class Thread
{
public:
    explicit Thread(const Callback& functor)
    {
        m_functor = functor;
        uv_thread_create(&m_handle, entry, this);
    }

    int join()
    {
        return uv_thread_join(&m_handle);
    }

private:
    uv_thread_t m_handle;
    Callback m_functor;
    static void entry(void* arg)
    {
        Thread* _this = (Thread *)arg;
        if (_this->m_functor)
        {
            _this->m_functor();
        }
        delete _this;
    }
    DISABLE_COPY(Thread);
};
}

#endif // UVPP_THREAD_H
