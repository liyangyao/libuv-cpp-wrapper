/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/24

****************************************************************************/

#include <QCoreApplication>
#include <QDebug>
#include "uvpp.h"
#include "thread.h"
#include "tcp.h"

void testServer()
{
    uv::Thread t([]()
    {
        uv::Loop loop;
        uv::Tcp server(&loop);
        server.bind("0.0.0.0", 8080);
        server.listen([&](int status)
        {
            if (status<0) return;
            uv::Tcp* client = new uv::Tcp(&loop);
            server.accept(client);
            client->read_start([client](const QByteArray &data)
            {
                qDebug()<<"recv:"<<data;
                client->write("I'm recved!!!", nullptr);

            });
            uv::Tcp *s = &server;
            client->setCloseCallback([client, s]()
            {
                qDebug()<<"closed";;
                delete client;
                s->close();
            });
        });
        loop.run();
    });
    //t.join();
}

void testClient()
{
    uv::Thread t([]()
    {
        uv::Loop loop;
        uv::Tcp* client = new uv::Tcp(&loop);
        client->connect("111.13.100.91", 80, [client](int status)
        {
            if (status==0)
            {
                client->write("GET / HTTP/1.1\r\nContent-Length: 0\r\n\r\n", nullptr);

                uv::Tcp* c = client;
                client->read_start([c](const QByteArray &data)
                {
                   qDebug()<<"Client Receive:"<<data;
                   delete c;
                });
            }
        });

        loop.run();
    });
    //t.join();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    testClient();
    testServer();

    qDebug()<<"after join";

    return a.exec();
}
