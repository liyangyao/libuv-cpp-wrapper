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

void Service::print(const QString &message)
{
    qDebug()<<"Service::print--->"<<message;
}
