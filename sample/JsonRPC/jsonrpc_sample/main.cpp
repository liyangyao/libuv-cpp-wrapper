/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/11/21

****************************************************************************/

#include <QCoreApplication>
#include "../jsonrpc.h"
#include <myservice.h>
#include <QDebug>

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::shared_ptr<MyService> svr(new MyService);
    jsonrpc::ServiceProvider sp;
    sp.regist(svr);


    {
        //1.invalid json
        QByteArray d = "{\"id\": 0, \"method\": \"method1\", \"params\"}";
        QJsonDocument doc = sp.deliverCall(d);

        qDebug()<<"RESULT:"<<doc.toJson(QJsonDocument::Compact);
    }

    {
        //找不到方法
        QByteArray d = "{\"id\": 1, \"method\": \"strange_method\", \"params\":[]}";
        QJsonDocument doc = sp.deliverCall(d);

        qDebug()<<"RESULT:"<<doc.toJson(QJsonDocument::Compact);
    }

    {
        //通知
        QByteArray d = "{\"id\": null, \"method\": \"notify\", \"params\":[\"Hello World!\"]}";
        QJsonDocument doc = sp.deliverCall(d);

        qDebug()<<"RESULT:"<<doc.toJson(QJsonDocument::Compact);
    }

    {
        //2. 按顺序的参数
        QByteArray d = "{\"id\": 2, \"method\": \"add\", \"params\":[101, 202]}";
        QJsonDocument doc = sp.deliverCall(d);

        qDebug()<<"RESULT:"<<doc.toJson(QJsonDocument::Compact);
    }

    {
        //.传送数组
        QByteArray d = "{\"id\": 3, \"method\": \"cat\", \"params\":[[\"s1\", \"s2\"]]}";
        QJsonDocument doc = sp.deliverCall(d);

        qDebug()<<"RESULT:"<<doc.toJson(QJsonDocument::Compact);
    }


    {
        //直接传对象
        QByteArray d = "{\"id\": 4, \"method\": \"add_by_object\", \"params\":{\"a\": 303, \"b\": 404}}";
        QJsonDocument doc = sp.deliverCall(d);

        qDebug()<<"RESULT:"<<doc.toJson(QJsonDocument::Compact);
    }
    return a.exec();
}

