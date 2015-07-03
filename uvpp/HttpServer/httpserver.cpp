/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/3

****************************************************************************/

#include "httpserver.h"
#include <QDebug>
#include <QDateTime>
#include "respon.h"

HttpServer::HttpServer():
    m_loop(),
    m_tcpServer(&m_loop)
{
    qDebug()<<"HttpServer::HttpServer";
}

void HttpServer::start(int port)
{
    qDebug()<<"HttpServer::start port="<<port;
    m_tcpServer.bind("0.0.0.0", port);
    m_tcpServer.listen(std::bind(&HttpServer::onNewConnection, this, std::placeholders::_1));
    m_loop.run();
}

void HttpServer::onNewConnection(int status)
{
    qDebug()<<"HttpServer::onNewConnection status="<<status;
    Connection* client = new Connection(&m_loop);
    m_tcpServer.accept(client);
    ResponPtr respon(new Respon);
    respon->connectEstablished(client);
    client->setContext(respon);

    client->read_start([client](const QByteArray &data)
    {
//        qDebug()<<"Read:"<<data;
        QString content = "Welcome!" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        client->context()->write(content.toUtf8());
    });
    client->setCloseCallback([client]()
    {
        client->context()->connectDestroyed();
        delete client;
    });
}
