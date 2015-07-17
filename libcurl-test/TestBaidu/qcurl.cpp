/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/2/5

****************************************************************************/

#include "qcurl.h"
#include <functional>

#pragma comment(lib,"libcurl.lib")



QCurl::QCurl(const QString &host, int port)
{
    m_websit = QString("http://%1:%2").arg(host).arg(port);
    m_handle = curl_easy_init();
    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
    setOptGzip();
}

QCurl::~QCurl()
{
    curl_easy_cleanup(m_handle);
}

//void QCurl::setCookie(const QString &cookie)
//{
//    curl_easy_setopt(m_handle, CURLOPT_COOKIE,  cookie.toAscii().data());
//}

//void QCurl::setCookieFileName(const QString &fileName)
//{
//    curl_easy_setopt(m_handle, CURLOPT_COOKIEFILE,  fileName.toAscii().data());
//    curl_easy_setopt(m_handle, CURLOPT_COOKIEJAR,  fileName.toAscii().data());
//}

void QCurl::setTimeout(int second)
{
    if (second<=0)
    {
        second = 1;
    }
    curl_easy_setopt(m_handle, CURLOPT_TIMEOUT, second);
}

QString QCurl::errorString()
{
    return curl_easy_strerror(m_lastRes);
}

void QCurl::setOptGzip()
{
    curl_easy_setopt(m_handle, CURLOPT_ENCODING, "gzip");
}

//void QCurl::setVerbose(bool enabled)
//{
//    curl_easy_setopt(m_handle, CURLOPT_VERBOSE, (int)enabled);
//}

bool QCurl::get(const QString &path)
{
    QString p = m_websit + path;
    curl_easy_setopt(m_handle, CURLOPT_URL, p.toUtf8().data());
    m_content.clear();    
    m_lastRes = curl_easy_perform(m_handle);
    return CURLE_OK==m_lastRes;
}

bool QCurl::post(const QString &path, const QString &data)
{
    QString p = m_websit + path;
    curl_easy_setopt(m_handle, CURLOPT_URL, p.toUtf8().data());
    QByteArray d = data.toUtf8();
    curl_easy_setopt(m_handle, CURLOPT_POST, d.length());
    curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, d.data());
    m_content.clear();
    m_lastRes = curl_easy_perform(m_handle);
    return CURLE_OK==m_lastRes;
}

bool QCurl::upload(QFile *f)
{
    curl_easy_setopt(m_handle, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(m_handle, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(m_handle, CURLOPT_READDATA, f);
    curl_easy_setopt(m_handle, CURLOPT_INFILESIZE_LARGE, f->size());
    m_content.clear();
    CURLcode res = curl_easy_perform(m_handle);
    return CURLE_OK==res;
}

//help   http://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
size_t QCurl::write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    QCurl *_this = (QCurl *)userdata;
    size_t length = size * nmemb;
    _this->m_content.append(ptr, length);
    return length;
}

size_t QCurl::read_callback(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    QFile *f = (QFile *)user_p;
    return f->read((char *)buffer, size*nmemb);
}
