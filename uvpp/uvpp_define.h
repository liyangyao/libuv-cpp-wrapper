/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#ifndef UVPP_DEFINE_H
#define UVPP_DEFINE_H

#include <memory>
#include <functional>
#include <QObject>
#include <libuv/include/uv.h>

namespace uvpp{

typedef std::function<void()> Callback;
typedef std::function<void(bool connected)> ConnectCallback;
typedef std::function<void(const QByteArray &data)> ReadCallback;
typedef std::function<void(int status)> WriteCallback;
typedef std::function<void(int status)> AfterWorkCallback;
typedef std::function<void(const QByteArray &data)> DataCallback;

#define DISABLE_COPY(Class) \
    explicit Class(const Class &); \
    Class &operator=(const Class &);
}

#endif // UVPP_DEFINE_H
