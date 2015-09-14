#include <QCoreApplication>
#include <QVariant>
#include <QMetaMethod>
#include <QDebug>
#include <QTextCodec>
#include <QVarLengthArray>
#include <QStringList>
#include "service.h"
#pragma execution_character_set("utf-8")

typedef QVariantMap Json;

enum ErrorCode {
    NoError         = 0,
    ParseError      = -32700,           // Invalid JSON was received by the server.
                                        // An error occurred on the server while parsing the JSON text.
    InvalidRequest  = -32600,           // The JSON sent is not a valid Request object.
    MethodNotFound  = -32601,           // The method does not exist / is not available.
    InvalidParams   = -32602,           // Invalid method parameter(s).
    InternalError   = -32603,           // Internal JSON-RPC error.
    ServerErrorBase = -32000,           // Reserved for implementation-defined server-errors.
    UserError       = -32099,           // Anything after this is user defined
    TimeoutError    = -32100
};
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

class Message
{
public:
    Message(const Json &json)
    {
        m_json = json;
        QString method = json["method"].toString();
//        QString svrName = method.section(".", 0, 0);
//        method = method.section(".", 1);
        //get signature
        QStringList paramTypes;
        foreach(const QVariant &v, json["params"].toList())
        {
            paramTypes.append(v.typeName());
        }
        m_signature = method + "(" + paramTypes.join(",") + ")";
        m_id = json["id"].toInt();
    }

    QVariant invoke(QObject* object)
    {
        const QMetaObject *metaObject = object->metaObject();
        for(int i=metaObject->methodOffset();  i<metaObject->methodCount(); i++)
        {
            QMetaMethod metaMethod = metaObject->method(i);
            QString signature = metaMethod.signature();

            if (signature==m_signature)
            {
                QVarLengthArray<void *, 10> parameters;

                QVariant returnValue(QMetaType::type(metaMethod.typeName()), nullptr);
                parameters.append(returnValue.data());
                QVariantList arguments = m_json["params"].toList();
                foreach(const QVariant &v, arguments)
                {
                    parameters.append(const_cast<void *>(v.constData()));
                }
                object->qt_metacall(QMetaObject::InvokeMetaMethod, i, parameters.data());
                return returnValue;
            }
        }
        return QVariant();
    }

private:
    Json m_json;
    QString m_signature;
    int m_id;
};



QVariant remoteCall(const QString &method, const QVariant &v1=QVariant(), const QVariant &v2=QVariant(), const QVariant &v3=QVariant(), const QVariant &v4=QVariant())
{
    Service service;
    Json request;
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

    Message r(request);
    return r.invoke(&service);
}

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QCoreApplication a(argc, argv);
    {
        QVariant returnValue = remoteCall("add", 32, 8, 9);
        qDebug()<<"ReturnValue="<<returnValue;
    }
    {
        QVariant returnValue = remoteCall("print", "Hello World!");
        qDebug()<<"ReturnValue="<<returnValue;
    }

    //    Service service;
    //    Json j;
    //    j["method"] = "Serverice.add";
    //    QVariantList list;
    //    list.append(9);
    //    list.append(2);
    //    j["params"] = list;
    //    call(&service, j);


    return a.exec();
}
