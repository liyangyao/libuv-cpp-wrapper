/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/11

****************************************************************************/

#ifndef THREAD_H
#define THREAD_H

#include "uvpp.h"

namespace uv{

class Thread: public Handle<uv_thread_t>
{
public:
    explicit Thread():
        Handle<uv_thread_t>()
    {

    }

    int create(const Functor& functor)
    {
        m_functor = functor;
        return uv_thread_create(get(), entry, this);
    }

    int join()
    {
        return uv_thread_join(get());
    }

private:
    Functor m_functor;
    static void entry(void* arg)
    {
        Thread* _this = (Thread *)arg;
        if (_this->m_functor)
        {
            _this->m_functor();
        }
    }
    DISABLE_COPY(Thread);
};

template <typename Value>
class Key
{
public:
    explicit Key()
    {
        uv_key_create(&m_key);
    }
    ~Key()
    {
        uv_key_delete(&m_key);
    }

    Value get()
    {
        Value v = (Value)(uv_key_get(&m_key));
        return v;
    }

    void set(const Value& v)
    {
        uv_key_set(&m_key, (void *)v);
    }

private:
    uv_key_t m_key;
    DISABLE_COPY(Key);
};

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

    void lock()
    {
        uv_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        uv_mutex_unlock(&m_mutex);
    }

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
}

#endif // THREAD_H
