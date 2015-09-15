#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include <QObject>
#include <QVariant>
#include <QVarLengthArray>
#include "result.h"
#include "message.h"

namespace JsonRpc
{

struct InvokeMetaMethod
{
    QObject* service;
    int methodIndex;
    int returnType;//QMetaType::type(metaMethod.typeName())
    void call(Message *message, Result *result)
    {
        QVarLengthArray<void *, 10> parameters;
        QVariant returnValue(returnType, nullptr);
        parameters.append(returnValue.data());
        foreach(const QVariant &v, message->params())
        {
            parameters.append(const_cast<void *>(v.constData()));
        }
        service->qt_metacall(QMetaObject::InvokeMetaMethod, methodIndex, parameters.data());
        result->setReturnValue(returnValue);
    }
};

class ServiceProvider
{
public:
    void registObject(QObject *service)
    {
        const QMetaObject *metaObject = service->metaObject();
        for(int i=metaObject->methodOffset();  i<metaObject->methodCount(); i++)
        {
            QMetaMethod metaMethod = metaObject->method(i);
            QString signature = metaMethod.signature();

            InvokeMetaMethod* m = new InvokeMetaMethod;
            m->methodIndex = i;
            m->returnType = QMetaType::type(metaMethod.typeName());
            m->service = service;
            m_signatures[signature] = m;
        }

    }

    void deliverCall(Message *message, Result *result)
    {
        result->setId(message->id());
        InvokeMetaMethod* m = m_signatures.value(message->signature(), nullptr);
        if (m)
        {
            m->call(message, result);
        }
        else{
            result->setMethodNotFound();
        }
    }

private:
    QHash<QString, InvokeMetaMethod*> m_signatures;

};
}

#endif // SERVICEPROVIDER_H
