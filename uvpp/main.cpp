/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#include "mainform.h"
#include <QApplication>
#include <QDebug>
#include <Windows.h>
#include "uvpp.h"


int main(int argc, char *argv[])
{
    uvpp::Thread thread([]()
    {
        uvpp::Loop loop;


        uvpp::TcpServer server(&loop);
        server.listen("0.0.0.0", 80);
        server.onNewConnection = [](const uvpp::ConnectionPtr &)
        {
            qDebug()<<"onNewConnection";
        };
        server.onMessage = [](const uvpp::ConnectionPtr &conn, const QByteArray &data)
        {
            qDebug()<<"onMessage";
            qDebug()<<data;
            qDebug()<<"-------------------------------";
            conn->write("HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\nHello World");

        };
        server.onConnectionClose = [](const uvpp::ConnectionPtr &)
        {
            qDebug()<<"onConnectionClose";
        };

        loop.run();
        qDebug("after run");
    });
    QApplication a(argc, argv);
    MainForm w;
    w.show();


    return a.exec();
}
