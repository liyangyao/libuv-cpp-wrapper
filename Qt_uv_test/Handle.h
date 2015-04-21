/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef HANDLE_H
#define HANDLE_H

#include "uvqt.h"
#include "loop.h"

namespace uv{

template <class Type>class Handle
{
public:
    Handle()
    {
        m_data = new Type;
        m_data->data = this;
    }

    virtual ~Handle()
    {
        qDebug()<<"~Handle";
        close();
    }

    Type *ptr()
    {
        return m_data;
    }

    uv_handle_t *handle()
    {
        return reinterpret_cast<uv_handle_t*>(m_data);
    }

    void close()
    {
        if (m_data)
        {
            uv_close(handle(), close_cb);
            m_data = nullptr;
        }
    }

private:
    Type *m_data;
    static void close_cb(uv_handle_t* handle)
    {
        qDebug()<<"release handle";
        delete handle;
    }
};
}

#endif // HANDLE_H
