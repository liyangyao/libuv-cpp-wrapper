#ifndef RESULT_H
#define RESULT_H

#include <QObject>
#include <QVariant>

namespace JsonRpc
{
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

class Result
{
public:
    Result():
        m_id(0),
        m_errorCode(0)

    {

    }

    void setId(int id)
    {
        m_id = id;
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
        m_errorCode = ParseError;
        m_errorMessage = QLatin1String("Parse error");
    }

    void setInvalidRequest()
    {
        m_errorCode = InvalidRequest;
        m_errorMessage = QLatin1String("Invalid Request");
    }

    void setMethodNotFound()
    {
        m_errorCode = MethodNotFound;
        m_errorMessage = QLatin1String("Method not found");
    }

    void setInvalidParams()
    {
        m_errorCode = InvalidParams;
        m_errorMessage = QLatin1String("Invalid params");
    }

    void setInternalError()
    {
        m_errorCode = InternalError;
        m_errorMessage = QLatin1String("Internal error");
    }

    void setReturnValue(const QVariant &value)
    {
        m_returnValue = value;
    }

    void print()
    {
        QVariantMap out;
        if (m_id>0)
        {
            out["id"] = m_id;
        }
        if (m_errorCode!=0)
        {
            QVariantMap error;
            error["code"] = m_errorCode;
            error["message"] = m_errorMessage;
            out["error"] = error;
        }
        else{
            out["result"] = m_returnValue;
        }
        qDebug()<<out;
    }

//    void toByteArray()
//    {
//        QVariantMap out;
//        out["id"] = m_id;
//        if (m_errorCode>0)
//        {
//            QVariantMap error;
//            error["code"] = m_errorCode;
//            error["message"] = m_errorMessage;
//        }
//        else{
//            out["result"] = m_returnValue;
//        }
//        qDebug()<<out;
//    }

private:
    int m_id;
    int m_errorCode;
    QString m_errorMessage;
    QVariant m_returnValue;
};
}

#endif // RESULT_H
