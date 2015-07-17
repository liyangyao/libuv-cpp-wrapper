/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/7

****************************************************************************/
#pragma execution_character_set("utf-8")

#include "widget.h"
#include <QApplication>
#include <QDebug>
#include <QUrl>
#include "qcurl.h"
#include <libcurl/curlpp.h>
#include <windows.h>

void testGet()
{
    curl::Easy request;
    request.setopt_url("http://www.baidu.com");
    request.setopt_writefunction([](char *ptr, size_t length)
    {
        //qDebug()<<"------------------------------------";
        //qDebug()<<QByteArray::fromRawData(ptr, length);
    });
    request.setopt_timeout(5);
    request.setopt_accept_encoding("gzip");
    if (!request.perform())
    {
        qDebug()<<"Error:"<<request.lastError();
    }
    qDebug()<<"CODE:"<<request.getinfo_respon_code();
    qDebug()<<"TIME:"<<request.getinfo_total_time();
    qDebug()<<"Content_type:"<<request.getinfo_content_type();
}

void testPost()
{
    curl::Easy request;
    request.setopt_url("HTTP://127.0.0.1/post");
    request.setopt_post_fields("{\"act\": \"pull\"}");
    request.setopt_writefunction([](char *ptr, size_t length)
    {
        //qDebug()<<"------------------------------------";
        qDebug()<<QByteArray::fromRawData(ptr, length);
    });
    if (!request.perform())
    {
        qDebug()<<"Error:"<<request.lastError();
    }
    ::Sleep(10*1000);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    curl::Global g;



    qDebug()<<"===========================================";
    qDebug()<< curl_version();
    qDebug()<<"===========================================";
    testPost();



    return a.exec();
}
