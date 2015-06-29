/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef HANDLE_H
#define HANDLE_H

#include "uvpp.h"

namespace uv{

template <typename HANDLE_T>
class Handle
{
public:
    explicit Handle()
    {
        m_uv_handle = new HANDLE_T;
        get<uv_handle_t>()->data = this;
    }

    virtual ~Handle()
    {
        close();
    }

    HANDLE_T *get()
    {
        return m_uv_handle;
    }


    HANDLE_T *ptr()
    {
        return m_uv_handle;
    }

    template <typename T>
    T *get()
    {
        return reinterpret_cast<T *>(m_uv_handle);
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
        if (m_uv_handle)
        {
            uv_close(get<uv_handle_t>(), close_cb);
            m_uv_handle = nullptr;
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
    HANDLE_T *m_uv_handle;
    static void close_cb(uv_handle_t* handle)
    {
        HANDLE_T* data = reinterpret_cast<HANDLE_T *>(handle);
        delete data;
    }
    DISABLE_COPY(Handle)
};
}

#endif // HANDLE_H
