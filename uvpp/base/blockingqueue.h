/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/29

****************************************************************************/

#ifndef UVPP_BLOCKINGQUEUE_H
#define UVPP_BLOCKINGQUEUE_H

#include <deque>
#include <libuv/uv.h>

template<typename T>
class BlockingQueue
{
public:
    BlockingQueue()
    {
        uv_mutex_init(&m_mutex);
        uv_cond_init(&m_cond);
    }

    void put(const T& x)
    {
        uv_mutex_lock(&m_mutex);
        m_queue.push_back(x);
        uv_cond_signal(&m_cond);
        uv_mutex_unlock(&m_mutex);
    }

    void put(T&& x)
    {
        uv_mutex_lock(&m_mutex);
        m_queue.push_back(std::move(x));
        uv_cond_signal(&m_cond);
        uv_mutex_unlock(&m_mutex);
    }


    T take()
    {
        uv_mutex_lock(&m_mutex);
        while(m_queue.empty())
        {
            uv_cond_wait(&m_cond, &m_mutex);
        }
        T front(std::move(m_queue.front()));
        m_queue.pop_front();
        uv_mutex_unlock(&m_mutex);
        return front;
    }

    size_t size() const
    {
        uv_mutex_lock(&m_mutex);
        int size = m_queue.size();
        uv_mutex_unlock(&m_mutex);
        return size;
    }

private:
    uv_mutex_t m_mutex;
    uv_cond_t m_cond;
    std::deque<T> m_queue;
};

#endif // BLOCKINGQUEUE_H
