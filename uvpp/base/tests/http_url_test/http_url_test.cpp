/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/30

****************************************************************************/

#include <QCoreApplication>
#include <QDebug>
#include <string>
#include <http-parser/http_parser.h>

static int parse_url(const char *url)
{
    struct http_parser_url u;
    if(0 == http_parser_parse_url(url, strlen(url), 0, &u))
    {
        if(u.field_set & (1 << UF_PORT))
        {
            qDebug()<<"port:"<< u.port;
        }
        else
        {
            qDebug()<<"port:"<< 80;
        }

        if(u.field_set & (1 << UF_HOST) )
        {
            std::string host(url+u.field_data[UF_HOST].off, u.field_data[UF_HOST].len);
            qDebug()<<"host:"<<QString::fromStdString(host);
        }

        if(u.field_set & (1 << UF_PATH))
        {
            QByteArray path(url+u.field_data[UF_PATH].off, u.field_data[UF_PATH].len);
            qDebug()<<"path:"<<path;
        }

        if(u.field_set & (1 << UF_QUERY) )
        {
            std::string query(url+u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len);
            qDebug()<<"query:"<<QString::fromStdString(query);
        }

        return 0;
    }

    return -1;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    parse_url("http://blog.csdn.net/foruok/article/details/8954726");
    qDebug()<<"------------";
    parse_url("http://www.baidu.com/s?wd=url&ie=UTF-8");

    return a.exec();
}
