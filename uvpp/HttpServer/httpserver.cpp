/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/3

****************************************************************************/

#include "httpserver.h"
#include <QDebug>
#include <QDateTime>
#include "respon.h"
#include "timer.h"
#include "async.h"
#include "thread.h"
#include <QMutexLocker>

HttpServer::HttpServer():
    m_loop(),
    m_tcpServer(&m_loop)
{
    qDebug()<<"HttpServer::HttpServer";
}

uv::Key<int> global_int;

void HttpServer::start(int port)
{

    global_int.set(100);
    qDebug()<<"Value="<< global_int.get();
    qDebug()<<"current tid="<<GetCurrentThreadId();
    uv::Thread* thread = new uv::Thread;
    thread->create([=]()
    {
        int v = global_int.get();
        global_int.set(v+1);
        qDebug()<<"THREAD Value="<<v ;

       qDebug()<<"1Run in new Thread tid="<<GetCurrentThreadId();
    });

    uv::Timer* timer = new uv::Timer(&m_loop);
    timer->start([=](){
        static int round = 0;
        round++;
        qDebug()<<"Timer round:"<<round;

        if (round==5)
        {
            uv::Async* async = new uv::Async(&m_loop,[=]()
            {
                qDebug()<<"Delete Timer";
                delete timer;
            });
            async->send();
        }
    }, 1000, 1000);
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
