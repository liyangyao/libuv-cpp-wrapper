/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/30

****************************************************************************/

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include "HttpParser.h"
#include <QUrl>

class Reqeust
{
public:
    QUrl url;
    QString method;
    QByteArray body;
};

class Respon
{

};

class HttpServer
{
public:
    HttpServer();
};

#endif // HTTPSERVER_H
