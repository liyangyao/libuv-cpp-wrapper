/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/10/9

****************************************************************************/

#include <QCoreApplication>
#include <jsonrpc.h>
#include <QDebug>

class JsonRPCService:public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void testNotification()
    {
        qDebug()<<"call testNotification";
    }
    Q_INVOKABLE void testNotification1(int n)
    {
        Q_ASSERT(n==1024);
        qDebug()<<"call testNotification1 n="<<n;
    }
    Q_INVOKABLE void testNotification3(const QString &msg, int n)
    {
        Q_ASSERT(n==1024);
        Q_ASSERT(msg=="HelloWorld");
        qDebug()<<"call testNotification3 msg="<<msg<<" n="<<n;
    }

    Q_INVOKABLE int call_add(int a, int b)
    {
        return a + b;
    }

    Q_INVOKABLE double call_add2(double a, double b)
    {
        return a + b;
    }
};
#include "jsonrpc_test_main.moc"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    jsonrpc::ServiceProvider sp;
    jsonrpc::ServicePtr svr(new JsonRPCService);
    sp.regist(svr);
    {
        jsonrpc::Message req("testNotification");
        QJsonDocument ret(sp.deliverCall(&req));
        qDebug()<<"---result---";
        qDebug()<<ret.toJson();
        qDebug()<<"------------";
    }
    {
        jsonrpc::Message req("testNotification1");
        req.appendParam(1024);
        QJsonDocument ret(sp.deliverCall(&req));
        qDebug()<<"---result---";
        qDebug()<<ret.toJson();
        qDebug()<<"------------";
    }
    {
        jsonrpc::Message req("testNotification3");
        req.appendParam("HelloWorld");
        req.appendParam(1024);
        QJsonDocument ret(sp.deliverCall(&req));
        qDebug()<<"---result---";
        qDebug()<<ret.toJson();
        qDebug()<<"------------";
    }

    {
        jsonrpc::Message req("call_add", 1008);
        req.appendParam(19);
        req.appendParam(33);
        QJsonDocument ret(sp.deliverCall(&req));
        Q_ASSERT(ret.object().value("id").toInt() == 1008);
        Q_ASSERT(ret.object().value("result").toInt() == 52);
        qDebug()<<"---result---";
        qDebug()<<ret.toJson();
        qDebug()<<"------------";
    }

    {
        jsonrpc::Message req("call_add2", 1009);
        req.appendParam(3.14);
        req.appendParam(4.2);
        QJsonDocument ret(sp.deliverCall(&req));
        Q_ASSERT(ret.object().value("id").toInt() == 1009);
        qDebug()<<"---result---";
        qDebug()<<ret.toJson();
        qDebug()<<"------------";
    }

    //batch call
    {
        jsonrpc::Message req1("call_add", 1008);
        req1.appendParam(19);
        req1.appendParam(33);

        jsonrpc::Message req2("call_add2", 1009);
        req2.appendParam(3.14);
        req2.appendParam(4.2);

        QJsonArray array;
        array.append(req1.toJsonObject());
        array.append(req2.toJsonObject());

        QJsonDocument doc(array);
        QJsonDocument ret = sp.deliverCall(doc.toJson());
        qDebug()<<"===result===";
        qDebug()<<ret.toJson();
    }

    return a.exec();
}
