/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/24

****************************************************************************/

#include <QCoreApplication>

#include <QDebug>
#include "httpparser.h"


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
        QByteArray d(
                    "GET /chat HTTP/1.1\r\n"
                    "Host: server.example.com\r\n"
                    "Upgrade: websocket\r\n"
                    "Connection: Upgrade\r\n"
                    "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                    "Origin: http://example.com\r\n"
                    "Sec-WebSocket-Protocol: chat, superchat\r\n"
                    "Sec-WebSocket-Version: 13\r\n\n");

        int n = hp.execute(d.constData(), d.length());
        if (n==d.length())
        {
            qDebug()<<n;
            qDebug()<<"method:"<<hp.method();
            qDebug()<<"upgrade:"<<hp.upgrade();
        }
        else{
            qDebug()<<"------ERROR---------";
            qDebug()<<hp.httpError();
        }
    }


    return a.exec();
}
