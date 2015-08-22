/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#ifndef UVPP_DEFINE_H
#define UVPP_DEFINE_H

#include <memory>
#include <functional>
#include <QObject>
#include <libuv/uv.h>

namespace uvpp{

//typedef std::function<void()> Callback;
typedef std::function<void()> NewConnectionCallback;
typedef std::function<void()> ShutdownCallback;
typedef std::function<void()> DisconnectCallback;
typedef std::function<void()> CloseCallback;
typedef std::function<void(bool connected)> ConnectCallback;
typedef std::function<void(const QByteArray &data)> ReadCallback;
typedef std::function<void(int status)> WriteCallback;
typedef std::function<void()> WorkCallback;
typedef std::function<void(int status)> AfterWorkCallback;
typedef std::function<void(const QByteArray &data)> DataCallback;
typedef std::function<void()> TimerCallback;
typedef std::function<void()> Functor;

#define UV_CONTAINER_OF(ptr, type, field)                                  \
      ((type *) ((char *) (ptr) - ((long) &((type *) 0)->field)))

#define DISABLE_COPY(Class) \
    explicit Class(const Class &); \
    Class &operator=(const Class &);
}

#endif // UVPP_DEFINE_H
