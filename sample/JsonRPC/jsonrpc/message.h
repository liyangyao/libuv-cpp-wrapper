#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QVariant>

namespace JsonRpc
{

typedef QVariantMap Json;

class Message
{
public:
    Message()
    {

    }

    Message(const Json &json):
        m_json(json)
    {

    }

    QVariantMap& value()
    {
        return m_json;
    }

    int id()
    {
        return m_json["id"].toInt();
    }

    QString method()
    {
        return m_json["method"].toString();
    }

    QVariantList params()
    {
        return m_json["params"].toList();
    }

    QString signature()
    {
        QStringList paramTypes;
        foreach(const QVariant &v, params())
        {
            paramTypes.append(v.typeName());
        }
        return method() + "(" + paramTypes.join(",") + ")";
    }

private:
    QVariantMap m_json;
};
}


#endif // MESSAGE_H
