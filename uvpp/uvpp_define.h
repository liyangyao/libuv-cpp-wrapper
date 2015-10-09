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
#include <QDebug>

namespace uv{

typedef std::function<void()> Callback;
typedef std::function<void(int status)> CallbackWithResult;
typedef std::function<void(const char *, int)> CallbackWithData;

#define UV_CONTAINER_OF(ptr, type, field)                                  \
      ((type *) ((char *) (ptr) - ((long) &((type *) 0)->field)))

#define DISABLE_COPY(Class) \
    explicit Class(const Class &); \
    Class &operator=(const Class &);
}

#endif // UVPP_DEFINE_H
