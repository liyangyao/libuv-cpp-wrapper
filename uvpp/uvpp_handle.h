/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#ifndef UVPP_HANDLE_H
#define UVPP_HANDLE_H

#include "uvpp_define.h"

namespace uvpp{
template <typename HANDLE_T>
class Handle
{
public:
    explicit Handle()
    {
        m_handle = new HANDLE_T;
    }

    virtual ~Handle()
    {
        close();
    }

    HANDLE_T *handle()
    {
        return m_handle;
    }

    template <typename T>
    T *handle()
    {
        return reinterpret_cast<T *>(m_handle);
    }

    bool is_active()
    {
        return uv_is_active(handle<uv_handle_t>());
    }

    bool is_closeing()
    {
        return uv_is_closing(handle<uv_handle_t>());
    }

    void ref()
    {
        uv_ref(handle<uv_handle_t>());
    }

    void unref()
    {
        uv_unref(handle<uv_handle_t>());
    }

    bool has_ref()
    {
        uv_has_ref(handle<uv_handle_t>());
    }

private:
    HANDLE_T *m_handle;

    void close()
    {
        uv_close(handle<uv_handle_t>(), close_cb);
    }

    static void close_cb(uv_handle_t* handle)
    {
        HANDLE_T* _handle = reinterpret_cast<HANDLE_T *>(handle);
        delete _handle;
    }
    DISABLE_COPY(Handle)
};
}


#endif // UVPP_HANDLE_H
