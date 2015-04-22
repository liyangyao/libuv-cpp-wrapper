/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef HANDLE_H
#define HANDLE_H

#include "uvpp.h"

namespace uv{

template <class Type>class Handle
{
public:
    explicit Handle()
    {
        m_data = new Type;
        reinterpret_cast<uv_handle_t*>(m_data)->data = this;
    }

    virtual ~Handle()
    {
        qDebug()<<"~Handle";
        close();
    }

    Type *get()
    {
        return m_data;
    }

    uv_handle_t *handle()
    {
        return reinterpret_cast<uv_handle_t*>(m_data);
    }

    int is_active()
    {
        return uv_is_active(handle());
    }

    int is_close()
    {
        return uv_is_closing(handle());
    }

    void close()
    {
        if (m_data)
        {
            uv_close(handle(), close_cb);
            m_data = nullptr;
        }
    }

    void ref()
    {
        uv_ref(handle());
    }

    void unref()
    {
        uv_unref(handle());
    }

    int has_ref()
    {
        uv_has_ref(handle());
    }

private:
    Type *m_data;
    static void close_cb(uv_handle_t* handle)
    {
        Type* data = reinterpret_cast<Type *>(handle);
        delete data;
    }
    DISABLE_COPY(Handle)
};
}

#endif // HANDLE_H
