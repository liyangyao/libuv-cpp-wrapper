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
        handle()->data = this;
    }

    virtual ~Handle()
    {
        if (is_closeing())
        {
            if (handle()->data)
            {
                handle()->data = nullptr;//wait close over
            }
            else{
                delete handle();//real closed
            }
        }
        else{
            handle()->data = nullptr;//handle_close_cb will not call m_onClose
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

    void close()
    {
        uv_close(handle<uv_handle_t>(), handle_close_cb);
    }

    void onClose(const CloseCallback &cb)
    {
        m_onClose = cb;
    }

private:
    HANDLE_T *m_handle;
    CloseCallback m_onClose;
    static void handle_close_cb(uv_handle_t* handle)
    {
        HANDLE_T* _handle = reinterpret_cast<HANDLE_T *>(handle);
        if (_handle->data)
        {
            Handle* _this = (Handle *)_handle->data;
            if (_this->m_onClose)
            {
                _this->m_onClose();
            }
            _handle->data = nullptr;//real closed
        }
        else{
            delete _handle;
        }
    }
    DISABLE_COPY(Handle)
};
}


#endif // UVPP_HANDLE_H
