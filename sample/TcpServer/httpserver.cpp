/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/30

****************************************************************************/

#include "httpserver.h"
#include <QDebug>

HttpServer::HttpServer()
{
    m_server.reset(new TcpServer(&m_loop));
}

void HttpServer::start(int port)
{
    bool success = m_server->listen("0.0.0.0", port);
    qDebug()<<"listen at"<<port<<success;
    m_server->connectionCallback = [this](const Connection::Ptr &conn)
    {
        ContextPtr context(new Context(conn.get()));
        conn->context = context;
        conn->messageCallback = std::bind(&Context::onMessage, context, std::placeholders::_1, std::placeholders::_2);
        context->httpRequestCallback = httpRequestCallback;
    };

    m_loop.run();
}



Context::Context(Connection* connection):
    m_respon(connection)
{
    qDebug()<<"Context";
    HttpParser* p = &m_parser;
    m_parser.onMessageBegin = [this]()->int
    {
        m_request.reset(new Reqeust);
        m_request->context = this;
        return 0;
    };
    m_parser.onUrl = [this](const char *at, size_t length)->int
    {
        m_request->urlData.append(at, length);
        return 0;
    };
    m_parser.onHeadersComplete = [this, p]()->int
    {
        m_request->url = QUrl(m_request->urlData);
        m_request->method = p->method();
        return 0;
    };

    m_parser.onBody = [this](const char *at, size_t length)->int
    {

        QByteArray d = QByteArray::fromRawData(at, length);
        m_request->body.append(d);
        return 0;
    };
    m_parser.onMessageComplete = [this]()->int
    {
        qDebug()<<"method:"<<m_request->method;
        qDebug()<<"url:"<<m_request->url;
        qDebug()<<"body:"<<m_request->body;
        if (httpRequestCallback)
        {
            httpRequestCallback(m_request.get(), &m_respon);
        };
        return 0;
    };
}

Context::~Context()
{
    qDebug()<<"~Context";
}

void Context::onMessage(const Connection::Ptr &, const QByteArray &d)
{
    int n = m_parser.execute(d.constData(), d.size());
    if (n!=d.size())
    {
        qDebug()<<"Error:";
        qDebug()<< m_parser.httpError();
    }
}


Respon::Respon(Connection *connection):
    m_connection(connection)
{

}

void Respon::writeBody(const QByteArray &d)
{
    QString head = QString("HTTP/1.1 200 OK\r\nContent-Length: %1\r\n\r\n").arg(d.size());
    m_connection->write(head.toUtf8());
    m_connection->write(d);
}
