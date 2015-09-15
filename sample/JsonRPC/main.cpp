#include <QCoreApplication>
#include <QVariant>
#include <QMetaMethod>
#include <QDebug>
#include <QTextCodec>
#include <QVarLengthArray>
#include <QStringList>
#include "service.h"
#include "jsonrpc/serviceprovider.h"
#pragma execution_character_set("utf-8")

/*
 * JsonMessage
Request,Respon,Notification,Error
*/

/*
 * 位置参数形式的rpc调用：
--> {"jsonrpc":"2.0", "method": "subtract", "params":[42, 23], "id": 1}
<-- {"jsonrpc": "2.0", "result": 19,"id": 1}

通知：

--> {"jsonrpc":"2.0", "method": "update", "params":[1,2,3,4,5]}
--> {"jsonrpc": "2.0", "method":"foobar"}

rpc调用一个不存在的方法：

--> {"jsonrpc":"2.0", "method": "foobar", "id":"1"}
<-- {"jsonrpc": "2.0", "error":{"code": -32601, "message": "Method not found"},"id": 1}

无效的JSON的rpc调用

rpc call with invalid JSON:

--> {"jsonrpc":"2.0", "method": "foobar, "params":"bar", "baz]
<-- {"jsonrpc": "2.0", "error":{"code": -32700, "message": "Parse error"},"id": null}

rpc call with invalid Request object:

无效请求对象的rpc调用：

--> {"jsonrpc":"2.0", "method": 1, "params": "bar"}
<-- {"jsonrpc": "2.0", "error":{"code": -32600, "message": "Invalid Request"},"id": null}
*/



void remoteCall(const QString &method, const QVariant &v1=QVariant(), const QVariant &v2=QVariant(), const QVariant &v3=QVariant(), const QVariant &v4=QVariant())
{
    Service service;
    JsonRpc::Json request;
    request["method"] = method;
    QVariantList params;
    if (!v1.isNull())
    {
        params.append(v1);
    }
    if (!v2.isNull())
    {
        params.append(v2);
    }
    if (!v3.isNull())
    {
        params.append(v3);
    }
    if (!v4.isNull())
    {
        params.append(v4);
    }
    request["params"] = params;
    request["id"] = 1001;

    JsonRpc::ServiceProvider provider;
    provider.registObject(&service);

    JsonRpc::Message message(request);
    JsonRpc::Result result;

    provider.deliverCall(&message, &result);
    result.print();
}

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QCoreApplication a(argc, argv);
    {
        remoteCall("add", 32, 16);
    }
//    {
//        remoteCall("print", "Helo");
//    }
    {
        remoteCall("print",  "Hello World");
    }



    return a.exec();
}
