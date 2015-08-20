/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/31

****************************************************************************/

#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <libuv/include/uv.h>

template <class T>
class Singleton
{
public:
    static T& instance()
    {
        uv_once(&m_once, init);
        return *(m_instance.get());
    }

private:
    Singleton()
    {

    }
    ~Singleton()
    {

    }

    static uv_once_t m_once;
    static std::unique_ptr<T> m_instance;
    static void init()
    {
        m_instance.reset(new T());
    }
};

template <class T>
uv_once_t Singleton<T>::m_once = UV_ONCE_INIT;

template <class T>
std::unique_ptr<T> Singleton<T>::m_instance;

#endif // SINGLETON_H
