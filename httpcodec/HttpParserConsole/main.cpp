/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/24

****************************************************************************/

#include <QCoreApplication>
#include <functional>
#include <http_parser.h>
#include <QDebug>

class HttpParser
{
public:
    typedef std::function<int()> HttpCallback;
    typedef std::function<int(const char *at, size_t length)> HttpDataCallback;
    explicit HttpParser()
    {
        m_parser.data = this;
    }

    size_t execute(const char *data, size_t len)
    {
        if (!m_settings)
        {
            m_settings.reset(new http_parser_settings);

            m_settings->on_message_begin = on_message_begin;
            m_settings->on_url = on_url;
            m_settings->on_header_field = on_header_field;
            m_settings->on_header_value = on_header_value;
            m_settings->on_headers_complete = on_headers_complete;
            m_settings->on_body = on_body;
            m_settings->on_message_complete = on_message_complete;
            http_parser_init(&m_parser, HTTP_REQUEST);

        }
        return http_parser_execute(&m_parser, m_settings.get(), data, len);
    }

    unsigned short statusCode()
    {
        return m_parser.status_code;
    }

    const char * method()
    {
        return http_method_str((http_method)m_parser.method);
    }

    HttpCallback onMessageBegin;
    HttpDataCallback onUrl;
    HttpDataCallback onHeaderField;
    HttpDataCallback onHeaderValue;
    HttpCallback onHeadersComplete;
    HttpDataCallback onBody;
    HttpCallback onMessageComplete;

private:
    http_parser m_parser;
    std::unique_ptr<http_parser_settings> m_settings;
    static int on_message_begin(http_parser* parser)
    {
        //qDebug()<<"on_message_begin";
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onMessageBegin)
        {
            return _this->onMessageBegin();
        }
        return 0;
    }

    static int on_url(http_parser* parser, const char *at, size_t length)
    {
        //qDebug()<<"on_url";
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onUrl)
        {
            return _this->onUrl(at, length);
        }
        return 0;
    }

    static int on_header_field(http_parser* parser, const char *at, size_t length)
    {
        //qDebug()<<"on_header_field";
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onHeaderField)
        {
            return _this->onHeaderField(at, length);
        }
        return 0;
    }

    static int on_header_value(http_parser* parser, const char *at, size_t length)
    {
        //qDebug()<<"on_header_value";
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onHeaderValue)
        {
            return _this->onHeaderValue(at, length);
        }
        return 0;
    }

    static int on_headers_complete(http_parser* parser)
    {
        //qDebug()<<"on_headers_complete";
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onHeadersComplete)
        {
            return _this->onHeadersComplete();
        }
        return 0;
    }

    static int on_body(http_parser* parser, const char *at, size_t length)
    {
        //qDebug()<<"on_body";
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onBody)
        {
            return _this->onBody(at, length);
        }
        return 0;
    }

    static int on_message_complete(http_parser* parser)
    {
        //qDebug()<<"on_message_complete";
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onMessageComplete)
        {
            return _this->onMessageComplete();
        }
        return 0;
    }

};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug()<<"RUN";

    HttpParser hp;
    hp.onBody=[](const char *at, size_t length)->int
    {

        qDebug()<<"BODY:"<<QByteArray(at, length);
        return 0;
    };

    hp.onMessageComplete=[]()->int
    {

        qDebug()<<"MessageComplete";
        return 0;
    };

    QByteArray d("GET / HTTP/1.1\r\nContent-Length: 5\r\n\r\nHello");
    int n = hp.execute(d.constData(), d.length());
    qDebug()<<n;
    qDebug()<<"method:"<<hp.method();


    return a.exec();
}
