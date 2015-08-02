/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/18

****************************************************************************/

#include <QCoreApplication>
#include "libuvpp.h"
#include <QDebug>

#include <queue>
#include <QTextCodec>
#include <QUuid>
#include "tcpserver.h"
#include "Singleton.h"
#include "httpserver.h"


class Session
{
public:
    Session()
    {
        uuid = QUuid::createUuid().toString();
        round = 0;
        qDebug()<<"Session:"<<uuid;
    }

    ~Session()
    {
        qDebug()<<"~Session"<<uuid;
    }

    int round;
    QString uuid;
};

class Foo
{
public:
    Foo()
    {
        qDebug()<<"Foo("<<this<<")Constructor";
    }
};

class Ufo
{
public:
    Ufo()
    {
        qDebug()<<"Ufo("<<this<<")Constructor";
    }
};


void testTcpServer()
{
    LoopEx loop;

    TcpServer server(&loop);
    server.listen("0.0.0.0", 80);
    server.messageCallback = ([](const Connection::Ptr &connection, const QByteArray &d)
    {
        std::shared_ptr<Session> session = std::static_pointer_cast<Session>(connection->context);
        session->round++;

        QByteArray respon = QString("%1 round=%2").arg(session->uuid).arg(session->round).toUtf8();
        QString head = QString("HTTP/1.1 200 OK\r\nContent-Length: %1\r\n\r\n").arg(respon.size());
        connection->write(head.toUtf8());
        connection->write(respon);
        if (session->round==2)
        {
            qDebug()<<"Call shutdown";
            connection->forceClose();
            return;
        }
    });
    server.connectionCallback = [](const Connection::Ptr &connection)
    {
        connection->context = std::shared_ptr<Session>(new Session);
        qDebug()<<"connectionCallback";
    };

    server.connectionCloseCallback = [](const Connection::Ptr &connection)
    {
        std::shared_ptr<Session> session = std::static_pointer_cast<Session>(connection->context);
        qDebug()<<"Session Close"<<session->uuid;
    };

    qDebug()<<"begin run";
    loop.run();
}

void testHttpServer()
{
    HttpServer s;
    s.httpRequestCallback = [](Reqeust* request, Respon* respon)
    {
        QString s = QString("method:%1 url:%2").arg(request->method).arg(QString(request->urlData));
        respon->writeBody(s.toUtf8());
    };

    s.start(80);
}

void testTcpServer2()
{
    LoopEx loop;

    TcpServer server(&loop);
    int port = 333;
    bool ok = server.listen("0.0.0.0", port);
    qDebug()<<"listen at"<<port<<ok;
    server.messageCallback = ([](const Connection::Ptr &connection, const QByteArray &d)
    {
        qDebug()<<"Connection("<<connection.get()<<") length="<<d.length()<<"hex:"<<d.toHex();

        connection->write(QByteArray::fromHex("ff"));
        connection->write(d);

    });
    server.connectionCallback = [](const Connection::Ptr &connection)
    {
        qDebug()<<"Connection("<<connection.get()<<") create";
    };

    server.connectionCloseCallback = [](const Connection::Ptr &connection)
    {
        qDebug()<<"Connection("<<connection.get()<<") close";
    };

    loop.run();
}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));   

    //testHttpServer();
    testTcpServer2();

    qDebug()<<"OVER";
    return a.exec();
}
