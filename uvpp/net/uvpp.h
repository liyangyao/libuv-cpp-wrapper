/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef UVQT_H
#define UVQT_H

#include <QObject>
#include <memory>
#include <functional>
#include <libuv/include/uv.h>
#include <QDebug>

using std::shared_ptr;
using std::unique_ptr;
using std::function;

#define DISABLE_COPY(Class) \
    Class(const Class &); \
    Class &operator=(const Class &);

namespace uv{
typedef function<void()> Callback;
typedef function<void()> Functor;
typedef function<void(int status)> RequestCallback;
typedef function<void(const QByteArray &data)> ReadCallback;

}

#endif // UVQT_H
