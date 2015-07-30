/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/30

****************************************************************************/

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include "HttpParser.h"
#include <QUrl>
#include "tcpserver.h"

class Reqeust
{
public:
    QUrl url;
    QString method;
    QByteArray body;
    QByteArray urlData;
};

class Respon
{
public:
    Respon(Connection* connection);
    void writeBody(const QByteArray &d);

private:
    Connection* m_connection;
};

class Context
{
public:
    Context(Connection* connection);
    ~Context();
    void onMessage(const Connection::Ptr &, const QByteArray &);
    std::function<void(Reqeust* request, Respon* respon)> httpRequestCallback;

private:
    HttpParser m_parser;
    std::unique_ptr<Reqeust> m_request;
    Respon m_respon;

};
typedef std::shared_ptr<Context> ContextPtr;

class HttpServer
{
public:
    explicit HttpServer();
    void start(int port);
    std::function<void(Reqeust* request, Respon* respon)> httpRequestCallback;

private:
    std::unique_ptr<TcpServer> m_server;
    LoopEx m_loop;
};

#endif // HTTPSERVER_H
