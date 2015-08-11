/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/10

****************************************************************************/

#ifndef RESPON_H
#define RESPON_H

#include <QObject>

namespace test
{
class Respon
{
public:
    void setStatusCode(int statusCode);
    void setHeader(const QString &field, const QString &value);
    void write(const QByteArray &data);
    void end(const QByteArray &data = "");
    void direct_write();//"Transfer-Encoding", "chunked"
};
}

#endif // RESPON_H
