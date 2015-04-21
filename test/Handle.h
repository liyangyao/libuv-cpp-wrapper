/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/19

****************************************************************************/

#ifndef HANDLE_H
#define HANDLE_H

namespace libuv {

template <class Type>class Handle
{
public:
    Handle()
    {
        m_data = new Type;
    }
    ~Handle()
    {
        uv_close(handle(), close_cb);
    }

    Type *data()
    {
        return m_data;
    }

    uv_handle_t *handle()
    {
        return reinterpret_cast<uv_handle_t*>(m_data);
    }

private:
    Type *m_data;
    static void close_cb(uv_handle_t* handle)
    {
        qDebug()<<"handle Release";
        delete handle;
    }
};
}

#endif // HANDLE_H
