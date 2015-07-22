/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/17

****************************************************************************/

#ifndef CURLPP_H
#define CURLPP_H

#include <functional>
#include <libcurl/curl.h>
#include <QObject>

#pragma execution_character_set("utf-8")

namespace curl
{
class Global
{
public:
    Global()
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    ~Global()
    {
        curl_global_cleanup();
    }
};

//class SList
//{
//public:
//    SList()
//    {
//        list = NULL;
//    }

//    void append(const char *s)
//    {
//        list = curl_slist_append(list, s);
//    }

//    ~SList()
//    {
//        curl_slist_free_all(list);
//    }

//    curl_slist *get()
//    {
//        return list;
//    }

//private:
//    struct curl_slist *list;
//};


typedef std::function<void(char *ptr, size_t length)> WriteFunctionCallback;
typedef std::function<int (void *buffer, size_t length)> ReadFunctionCallback;
class Easy
{
public:
    explicit Easy()
    {
        m_handle = curl_easy_init();
    }

    bool perform()
    {
         m_lastError = curl_easy_perform(m_handle);
         //m_headers.reset();
         return m_lastError==CURLE_OK;
    }

    void reset()
    {
        curl_easy_reset(m_handle);
    }

    bool setopt_url(const char *url)
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_URL, url);
        return m_lastError==CURLE_OK;
    }

    bool setopt_httpget()
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1);
        return m_lastError==CURLE_OK;
    }

    bool setopt_httppost()
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_HTTPPOST, 1);
        return m_lastError==CURLE_OK;
    }

    bool setopt_header()
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_HEADER);
        return m_lastError==CURLE_OK;
    }

    bool setopt_post_field_size(int size)
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_POSTFIELDSIZE, size);
        return m_lastError==CURLE_OK;
    }

    bool setopt_post_fields(const char *postdata)
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, postdata);
        return m_lastError==CURLE_OK;
    }



    bool setopt_accept_encoding(const char* acceptEncoding)
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_ACCEPT_ENCODING, acceptEncoding);
        return m_lastError==CURLE_OK;
    }

    bool setopt_timeout(int second)
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_TIMEOUT, second);
        return m_lastError==CURLE_OK;
    }

    bool setopt_connect_timeout(int second)
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_CONNECTTIMEOUT, second);
        return m_lastError==CURLE_OK;
    }

    bool setopt_user_agent(const char* userAgent)
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_USERAGENT, userAgent);
        return m_lastError==CURLE_OK;
    }

    //Http返回数据的写函数
    bool setopt_writefunction(const WriteFunctionCallback &writeFunction)
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, writefunction);
        if (m_lastError==CURLE_OK)
        {
            curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
            m_writeFunction = writeFunction;
        }
        return m_lastError==CURLE_OK;;
    }

    //POST请求时,读取数据的读函数
    bool setopt_readfunction(const ReadFunctionCallback &readFunction)
    {
        m_lastError = curl_easy_setopt(m_handle, CURLOPT_READFUNCTION, writefunction);
        if (m_lastError==CURLE_OK)
        {
            curl_easy_setopt(m_handle, CURLOPT_READDATA, this);
            m_readFunction = readFunction;
        }
        return m_lastError==CURLE_OK;;
    }

//    bool setopt_httpheader(const QStringList& list)
//    {
//        if (!m_headers)
//        {
//            m_headers = std::unique_ptr<SList>(new SList);
//        }
//        for(int i=0; i<list.size(); i++)
//        {
//            m_headers->append(list[i].toUtf8().constData());
//        }
//        m_lastError = curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, m_headers->get());
//    }


    char * getinfo_content_type()
    {
        char *content_type = NULL;
        m_lastError = curl_easy_getinfo(m_handle, CURLINFO_CONTENT_TYPE, &content_type);
        return content_type;
    }

    int getinfo_respon_code()
    {
        long code = 0;
        m_lastError = curl_easy_getinfo(m_handle, CURLINFO_RESPONSE_CODE, &code);
        return code;
    }

    double getinfo_total_time()
    {
        double time = 0;
        m_lastError = curl_easy_getinfo(m_handle, CURLINFO_TOTAL_TIME, &time);
        return time;
    }

    const char *lastError()
    {
        return curl_easy_strerror(m_lastError);
    }

    ~Easy()
    {
        curl_easy_cleanup(m_handle);
    }

    CURL *handle()
    {
        return m_handle;
    }

private:
    CURL *m_handle;
    CURLcode m_lastError;
    //std::unique_ptr<SList> m_headers;
    WriteFunctionCallback m_writeFunction;
    ReadFunctionCallback m_readFunction;
    static size_t writefunction(char *buffer, size_t size, size_t count, void *user)
    {
        Easy *_this = (Easy *)user;
        size_t length = size * count;
        if (_this->m_writeFunction)
        {
            _this->m_writeFunction(buffer, length);
        }
        return length;
    }

    static size_t readFunction(void *buffer, size_t size, size_t count, void *user)
    {
        Easy *_this = (Easy *)user;
        size_t length = size * count;
        if (_this->m_readFunction)
        {
            return _this->m_readFunction(buffer, length);
        }
        return 0;
    }
};

//    curl_easy_setopt(request.handle(), CURLOPT_COOKIEFILE, "");
//-----------cookie----------------
//struct curl_slist *cookies = NULL;
//curl_easy_getinfo(request.handle(), CURLINFO_COOKIELIST, &cookies);		//获得cookie数据

//int i=1;
//while (cookies)
//{
//    qDebug("[%d]: %s", i, cookies->data);
//    cookies = cookies->next;
//    i++;
//}


//  sample
//-----------------------------
//curl::Global g;
//curl::Easy request;
//request.setopt_url("http://www.baidu.com");
//request.setopt_writefunction([](char *ptr, size_t length)
//{
//    //qDebug()<<"------------------------------------";
//    qDebug()<<QByteArray::fromRawData(ptr, length);
//});
//request.setopt_timeout(5);
//if (!request.perform())
//{
//    qDebug()<<"Error:"<<request.lastError();
//}
//qDebug()<<"CODE:"<<request.getinfo_respon_code();
//qDebug()<<"TIME:"<<request.getinfo_total_time();
//qDebug()<<"Content_type:"<<request.getinfo_content_type();


//qDebug()<<"===========================================";
}

#endif // CURLPP_H
