#include "service.h"
#include <QDebug>

Service::Service(QObject *parent) :
    QObject(parent)
{
}

int Service::add(int a, int b)
{
    int v = a + b;
    qDebug()<<"call Service::add "<<v;
    return v;
}

int Service::add(int a, int b, int c)
{
    int v = a + b + c;
    qDebug()<<"call Service(3)::add "<<v;
    return v;
}

void Service::print(const QString &message)
{
    qDebug()<<"Service::print--->"<<message;
}
