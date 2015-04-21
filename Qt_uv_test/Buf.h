/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#ifndef BUF_H
#define BUF_H

#include "uvqt.h"
#include "loop.h"

namespace uv{
class Buf
{
public:
    Buf(const QByteArray& ba):
        m_byteArray(ba)
    {
        uv_buf_init(ba.constData(), ba.length());
    }

    uv_buf_t* ptr()
    {
        return &m_buf;
    }

private:
    uv_buf_t m_buf;
    QByteArray m_byteArray;
};
}

#endif // BUF_H
