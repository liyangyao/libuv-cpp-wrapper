/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/3

****************************************************************************/

#ifndef RESPON_H
#define RESPON_H

#include <QObject>
#include <tcp.h>


class Respon;
class Request;
typedef std::shared_ptr<Respon> ResponPtr;
typedef std::shared_ptr<Request> RequestPtr;
typedef uv::Tcp<ResponPtr> Connection;
//typedef std::function<void(const RequestPtr &request, const ResponPtr &respon)> HttpRequestCallback;

//class Request
//{
//public:
//    QString path;
//    QUrl url;
//};

class Respon
{
public:
    Respon();
    ~Respon();
    void connectEstablished(Connection* connection);
    void connectDestroyed();

    void write(const QByteArray &d);
    void close();

private:
    Connection* m_connection;
    //HttpRequestCallback m_callback;
};


#endif // RESPON_H
