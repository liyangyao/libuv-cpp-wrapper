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




int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

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

    qDebug()<<"OVER";
    return a.exec();
}
