/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/9/28

****************************************************************************/

#ifndef LI_JSONRPC_H
#define LI_JSONRPC_H

#include <memory>
#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QHash>
#include <QMetaMethod>

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

namespace jsonrpc{

typedef QObject Service;
typedef std::shared_ptr<Service> ServicePtr;

namespace inner
{
enum ErrorCode {
    NoError         = 0,
    ParseError      = -32700,           // Invalid JSON was received by the server.
    InvalidRequest  = -32600,           // The JSON sent is not a valid Request object.
    MethodNotFound  = -32601,           // The method does not exist / is not available.
    InvalidParams   = -32602,           // Invalid method parameter(s).
    InternalError   = -32603,           // Internal JSON-RPC error.
    ServerErrorBase = -32000,           // Reserved for implementation-defined server-errors.
    UserError       = -32099,           // Anything after this is user defined
    TimeoutError    = -32100
};

class Util
{
public:
    static QString JSTypeToString(int type)
    {
        switch (type) {
        case QJsonValue::Null:
            return "Null";
            break;
        case QJsonValue::Bool:
            return "Bool";
            break;
        case QJsonValue::Double:
            return "Double";
            break;
        case QJsonValue::String:
            return "String";
            break;
        case QJsonValue::Array:
            return "Array";
            break;
        case QJsonValue::Object:
            return "Object";
            break;
        default:
            return "Undefined";
            break;
        }
    }

    static QVariant convertJSValueToVariant(int type, const QJsonValue &value)
    {
        switch (type) {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Long:
        case QMetaType::LongLong:
        case QMetaType::Short:
        case QMetaType::Char:
        case QMetaType::ULong:
        case QMetaType::ULongLong:
        case QMetaType::UShort:
        case QMetaType::UChar:
            return (int)value.toDouble();
        case QMetaType::Float:
        case QMetaType::Double:
            return value.toDouble();
        case QMetaType::QVariantList:
            return value.toArray().toVariantList();
        case QMetaType::QStringList:
        case QMetaType::QVariantMap:
            return value.toObject().toVariantMap();
        case QMetaType::QString:
            return value.toString();
        case QMetaType::Bool:
            return value.toBool();
        default:
            break;
        }

        return QJsonValue();
    }

    static int convertVariantTypeToJSType(int type)
    {
        switch (type) {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Double:
        case QMetaType::Long:
        case QMetaType::LongLong:
        case QMetaType::Short:
        case QMetaType::Char:
        case QMetaType::ULong:
        case QMetaType::ULongLong:
        case QMetaType::UShort:
        case QMetaType::UChar:
        case QMetaType::Float:
            return QJsonValue::Double;    // all numeric types in js are doubles
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            return QJsonValue::Array;
        case QMetaType::QVariantMap:
            return QJsonValue::Object;
        case QMetaType::QString:
            return QJsonValue::String;
        case QMetaType::Bool:
            return QJsonValue::Bool;
        default:
            break;
        }

        return QJsonValue::Undefined;
    }

    static QString signatureFromJsonArray(const QJsonArray &array)
    {
        QStringList list;
        foreach(const QJsonValue &v, array)
        {
            list<<JSTypeToString(v.type());
        }
        return list.join(",");
    }

    static QString signatureFromMethod(const QMetaMethod &method)
    {
        QStringList list;
        for(int i=0; i<method.parameterCount(); i++)
        {
            int type = method.parameterType(i);
            list<<JSTypeToString(convertVariantTypeToJSType(type));
        }
        return list.join(",");
    }

    static QVariantList createParameterVariantList(const QMetaMethod &method, const QJsonArray &array)
    {
        QVariantList varList;
        for(int i=0; i<method.parameterCount(); i++)
        {
            int type = method.parameterType(i);
            QVariant v= convertJSValueToVariant(type, array.at(i));
            varList.append(v);
        }
        return varList;
    }

    static QVariant invoke(QObject* object, const QMetaMethod &method, const QJsonArray &params)
    {
        QVarLengthArray<void *, 10> invokeParams;
        //压入返回值,参数
        int returnType = method.returnType();
        if (returnType==QMetaType::Void)
        {
            returnType = QMetaType::UnknownType;
        }
        QVariant returnValue(returnType, nullptr);
        invokeParams.append(returnValue.data());
        //压入参数
        QVariantList varList = inner::Util::createParameterVariantList(method, params);
        foreach(const QVariant &v, varList)
        {
            invokeParams.append(const_cast<void *>(v.constData()));
        }
        //调用
        object->qt_metacall(QMetaObject::InvokeMetaMethod, method.methodIndex(), invokeParams.data());
        return returnValue;
    }
};
}

class Request
{
public:
    Request(const QString& method, int id=0)
    {
        QJsonObject root;
        root["method"] = method;
        if (id>0)
        {
            root["id"] = id;
        }
        else{
            root["id"] = QJsonValue();
        }
        root["params"] = m_params;
        m_json.setObject(root);
    }

    void appendParam(const QVariant &v)
    {
        m_params.push_back(QJsonValue::fromVariant(v));
    }

    QJsonDocument& json()
    {
        return m_json;
    }

private:
    QJsonDocument m_json;
    QJsonArray m_params;
};

class Message
{
public:
    Message(const QJsonObject &jsonObject):
        m_root(jsonObject),
        m_params(jsonObject["params"].toArray())
    {
        m_id = m_root["id"].toInt();
        m_method = m_root["method"].toString();
        m_isValid = !m_method.isEmpty();
    }

    Message(const QString& method, int id=0):
        m_method(method),
        m_id(id)
    {
        m_root["method"] = method;
        if (id>0)
        {
            m_root["id"] = id;
        }
        else{
            m_root["id"] = QJsonValue();
        }
        m_root["params"] = m_params;
        m_isValid = !m_method.isEmpty();
    }

    void appendParam(const QVariant &v)
    {
        m_params.push_back(QJsonValue::fromVariant(v));
        m_root["params"] = m_params;
    }

    QJsonObject toJsonObject()
    {        
        return m_root;
    }

    QJsonDocument toJsonDocument()
    {
        return QJsonDocument(m_root);
    }

    bool isValid()
    {
        return m_isValid;
    }

    int id()
    {
        return m_id;
    }

    bool isNotification()
    {
        return m_id<=0;
    }

    QString method()
    {
        return m_method;
    }

    QJsonArray params()
    {
        return m_params;
    }

    QString signature()
    {
        return method() + "(" + inner::Util::signatureFromJsonArray(params()) + ")";
    }

private:
    QJsonObject m_root;
    QJsonArray m_params;
    int m_id;
    QString m_method;
    bool m_isValid;
};

class Result
{
public:
    Result(int id = NULL):
        m_id(id),
        m_errorCode(0)

    {

    }

    int id()
    {
        return m_id;
    }

    int errorCode()
    {
        return m_errorCode;
    }

    QString errorMessage()
    {
        return m_errorMessage;
    }

    QVariant returnValue()
    {
        return m_returnValue;
    }

    void setParseError()
    {
        m_errorCode = inner::ParseError;
        m_errorMessage = QLatin1String("Parse error");
    }

    void setInvalidRequest()
    {
        m_errorCode = inner::InvalidRequest;
        m_errorMessage = QLatin1String("Invalid Request");
    }

    void setMethodNotFound()
    {
        m_errorCode = inner::MethodNotFound;
        m_errorMessage = QLatin1String("Method not found");
    }

    void setInvalidParams()
    {
        m_errorCode = inner::InvalidParams;
        m_errorMessage = QLatin1String("Invalid params");
    }

    void setInternalError()
    {
        m_errorCode = inner::InternalError;
        m_errorMessage = QLatin1String("Internal error");
    }

    void setReturnValue(const QVariant &value)
    {
        m_returnValue = value;
    }

    QJsonObject value()
    {
        QJsonObject root;
        if (m_id<=0)
        {
            root["id"] = QJsonValue();
        }
        else{
            root["id"] = m_id;
        }
        if (m_errorCode!=0)
        {
            QJsonObject error;
            error["code"] = m_errorCode;
            error["message"] = m_errorMessage;
            root["error"] = error;
        }
        else{
            root["result"] = QJsonValue::fromVariant(m_returnValue);
        }
        return root;
    }

private:
    int m_id;
    int m_errorCode;
    QString m_errorMessage;
    QVariant m_returnValue;
};

class ServiceProvider
{
public:
    struct ServiceMethod
    {
        ServicePtr service;
        QMetaMethod method;
    };
    typedef std::shared_ptr<ServiceMethod> ServiceMethodPtr;

    void regist(const ServicePtr &service)
    {
        auto metaObject = service->metaObject();
        for(int index = metaObject->methodOffset(); index < metaObject->methodCount(); index++)
        {
            QMetaMethod method = metaObject->method(index);
            QString methodSignature = method.name() + "(" + inner::Util::signatureFromMethod(method) + ")";
            ServiceMethodPtr m(new ServiceMethod);
            m->service = service;
            m->method = method;
            m_serviceMethods.insert(methodSignature, m);
        }
    }

    QJsonObject deliverCall(Message *message)
    {
        Result result(message->id());
        ServiceMethodPtr serviceMethod = m_serviceMethods.value(message->signature(), nullptr);

        if (serviceMethod)
        {
            QVariant returnValue = inner::Util::invoke(serviceMethod->service.get(), serviceMethod->method, message->params());
            result.setReturnValue(returnValue);
        }
        else{
            result.setMethodNotFound();
        }
        return result.value();
    }

    QJsonDocument deliverCall(const QByteArray &data)
    {
        QJsonDocument json= QJsonDocument::fromJson(data);
        if (json.isObject())
        {
            QJsonObject ret = jsonrpcObject(json.object());
            if (!ret.isEmpty())
            {
                return QJsonDocument(ret);
            }
            return QJsonDocument();
        }
        if (json.isArray())
        {
           return jsonrpcArray(json.array());
        }
        jsonrpc::Result result;
        result.setParseError();
        return QJsonDocument(result.value());
    }

private:
    QHash<QString, ServiceMethodPtr> m_serviceMethods;
    QJsonObject jsonrpcObject(const QJsonObject &object)
    {
        jsonrpc::Message msg(object);
        if (!msg.isValid())
        {
            jsonrpc::Result result;
            result.setInvalidRequest();
            return result.value();
        }
        return deliverCall(&msg);
    }

    QJsonDocument jsonrpcArray(const QJsonArray &array)
    {
        if (array.isEmpty())
        {
            jsonrpc::Result result(0);
            result.setInvalidRequest();
            return QJsonDocument(result.value());
        }
        QJsonArray returnArray;
        foreach(const QJsonValue &value, array)
        {
            if (!value.isObject())
            {
                jsonrpc::Result result;
                result.setInvalidRequest();
                returnArray.append(result.value());
            }
            else{
                QJsonObject ret = jsonrpcObject(value.toObject());
                if (!ret.isEmpty())
                {
                    returnArray.append(ret);
                }
            }
        }

        if (!returnArray.isEmpty())
        {
            return QJsonDocument(returnArray);
        }
        else{
            return QJsonDocument();
        }
    }
};
}
#endif // LI_JSONRPC_H
