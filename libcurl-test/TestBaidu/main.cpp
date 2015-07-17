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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
//    QCurl u("www.baidu.com", 80);
//    u.setTimeout(3);
//    if (u.get("/"))
//    {
//        qDebug()<<"Respon size="<<u.responContent().size();
//        QString s = u.responContent();
//        qDebug()<<s;
//    }
//    else{
//        qDebug()<<"Error:"<<u.errorString();
//    }
    curl::Global g;
    curl::Easy request;
    request.setopt_url("http://www.baidu.com");
    request.setopt_writefunction([](char *ptr, size_t length)
    {
        //qDebug()<<"------------------------------------";
        qDebug()<<QByteArray::fromRawData(ptr, length);
    });
    request.setopt_timeout(5);
    if (!request.perform())
    {
        qDebug()<<"Error:"<<request.lastError();
    }
    qDebug()<<"CODE:"<<request.getinfo_respon_code();
    qDebug()<<"TIME:"<<request.getinfo_total_time();
    qDebug()<<"Content_type:"<<request.getinfo_content_type();


    qDebug()<<"===========================================";



    return a.exec();
}
