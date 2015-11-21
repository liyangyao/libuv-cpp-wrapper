/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/11/21

****************************************************************************/

#include "myservice.h"
#include <QDebug>

MyService::MyService(QObject *parent) : QObject(parent)
{

}

int MyService::add(int a, int b)
{
    return a + b;
}

int MyService::add_by_object(QVariantMap map)
{
    int a = map["a"].toInt();
    int b = map["b"].toInt();
    return a + b;
}

QString MyService::cat(const QStringList &list)
{
    return list.join("*");
}

QString MyService::cat2(const QVariantList &list)
{
    QStringList out;
    foreach(const QVariant &v, list)
    {
        out.append(v.toString());
    }
    return out.join("+");
}

void MyService::notify(const QString &msg)
{
    qDebug()<<"---Start Notify---";
    qDebug()<<msg;
    qDebug()<<"---End Notify---";
}

