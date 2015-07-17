/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/2/5

****************************************************************************/

#ifndef QCURL_H
#define QCURL_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <libcurl/curl.h>
#include <functional>
#include <QStringList>

#pragma execution_character_set("utf-8")

class QCurl
{
public:
    explicit QCurl(const QString &host, int port);
    ~QCurl();
    bool get(const QString &path);
    bool post(const QString &path, const QString& data);
    QByteArray& responContent()
    {
        return m_content;
    }

    bool upload(QFile* f);//未测试
    void setTimeout(int second);
    QString errorString();

private:
    CURL *m_handle;
    QString m_websit;
    QByteArray m_content;
    CURLcode m_lastRes;
    void setOptGzip();
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t read_callback(void *buffer, size_t size, size_t nmemb, void *user_p);

};



#endif // QCURL_H
