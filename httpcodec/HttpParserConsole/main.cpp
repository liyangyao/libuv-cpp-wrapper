/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/24

****************************************************************************/

#include <QCoreApplication>

#include <QDebug>
#include <QThread>
#include "httpparser.h"
#include "BlockingQueue.h"

BlockingQueue<QString> q;

class ThreadA:public QThread
{
    void run()
    {
        while(true)
        {
            QString s = q.take();
            qDebug()<<"take:"<<s;
            ::Sleep(1500);
        }
    }
};

class ThreadB:public QThread
{
    void run()
    {
        int n = 0;
        while(true)
        {
            QString s = QString("Round %1").arg(n);
            qDebug()<<"pus:"<<s;
            q.put(s);
            ::Sleep(1000);
            n++;
            if (n==10)
            {
                break;
            }
        }
    }
};

void testBQ()
{
    (new ThreadA)->start();
    (new ThreadB)->start();
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug()<<"RUN";

    HttpParser hp;
    hp.onBody=[](const char *at, size_t length)->int
    {

        qDebug()<<"BODY:"<<QByteArray(at, length);
        return 0;
    };

    hp.onMessageComplete=[]()->int
    {

            qDebug()<<"MessageComplete";
            return 0;
};

    //    {
    //        QByteArray d("GET / HTTP/1.1\r\nContent-Length: 5\r\n\r\nHello");
    //        int n = hp.execute(d.constData(), d.length());
    //        qDebug()<<n;
    //        qDebug()<<"method:"<<hp.method();
    //    }

    {
        QByteArray d("GET / HTTP/1.1\r\nContent-Length: 5\r\nConnection: close\r\n\r\nHello");
//        QByteArray d(
//                    "GET /chat HTTP/1.1\r\n"
//                    "Host: server.example.com\r\n"
//                    "Upgrade: websocket\r\n"
//                    "Connection: Upgrade\r\n"
//                    "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
//                    "Origin: http://example.com\r\n"
//                    "Sec-WebSocket-Protocol: chat, superchat\r\n"
//                    "Sec-WebSocket-Version: 13\r\n\n");

        int n = hp.execute(d.constData(), d.length());
        if (n==d.length())
        {
            qDebug()<<n;
            qDebug()<<"method:"<<hp.method();
            qDebug()<<"upgrade:"<<hp.upgrade();
            qDebug()<<"keepAlive:"<<hp.keepAlive();
        }
        else{
            qDebug()<<"------ERROR---------";
            qDebug()<<hp.httpError();
        }
    }

    testBQ();



    return a.exec();
}
