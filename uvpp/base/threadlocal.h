#ifndef UVPP_THREADLOCAL_H
#define UVPP_THREADLOCAL_H
namespace uvpp {

#include "libuv/uv.h"
#include "uvpp/uvpp_define.h"

template<typename T>
class ThreadLocal{
public:
    explicit ThreadLocal()
    {

    }

    ~ThreadLocal()
    {

    }

    T& value()
    {
        T* perThreadValue = static_cast<T*>(uv_key_get(&m_key));
        if (!perThreadValue)
        {
            T* newObj = new T();
            uv_key_set(&m_key, newObj);
            perThreadValue = newObj;
        }
        return *perThreadValue;
    }

    void reset()
    {
        T* perThreadValue = static_cast<T*>(uv_key_get(&m_key));
        if (perThreadValue)
        {
            delete perThreadValue;
        }
        uv_key_set(&m_key, nullptr);
    }

private:
    uv_key_t m_key;
    DISABLE_COPY(ThreadLocal)
};

}
#endif // THREADLOCAL_H
