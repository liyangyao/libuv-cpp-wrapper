/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#ifndef UVPP_HANDLE_H
#define UVPP_HANDLE_H

#include "uvpp_define.h"

namespace uv{
template <typename HANDLE_T>
class Handle
{
public:
    explicit Handle()
    {
        m_handle = new HANDLE_T;
        handle()->data = this;
    }

    virtual ~Handle()
    {
        if (is_closed())
        {
            delete handle();
            return;
        }
        handle()->data = nullptr;
        if (!is_closing())
        {
            close();
        }
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

    bool is_closing()
    {
        return uv_is_closing(handle<uv_handle_t>());
    }

    bool is_closed()
    {
        return nullptr==handle()->data;
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

    void close()
    {
        uv_close(handle<uv_handle_t>(), handle_close_cb);
    }

    void onClose(const Callback &closeCallback)
    {
        m_closeCallback = closeCallback;
    }

private:
    HANDLE_T *m_handle;
    Callback m_closeCallback;
    static void handle_close_cb(uv_handle_t* handle)
    {
        HANDLE_T* _handle = reinterpret_cast<HANDLE_T *>(handle);
        if (_handle->data)
        {
            Handle* _this = (Handle *)_handle->data;
            Callback cb = _this->m_closeCallback;
            handle->data = nullptr;
            _this->m_closeCallback = nullptr;
            if (cb)
            {
                cb();
            }
        }
        else{
            delete _handle;
        }
    }
    DISABLE_COPY(Handle)
};
}


#endif // UVPP_HANDLE_H
