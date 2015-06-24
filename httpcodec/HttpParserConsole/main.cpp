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



        }
        return http_parser_execute(&m_parser, m_settings.get(), data, len);
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

    static std::unique_ptr<http_parser_settings> m_settings;
    static int on_message_begin(http_parser* parser)
    {
        qDebug()<<"BEGIN";
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onMessageBegin)
        {
            return _this->onMessageBegin();
        }
        return 0;
    }

    static int on_url(http_parser* parser, const char *at, size_t length)
    {
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onUrl)
        {
            return _this->onUrl(at, length);
        }
        return 0;
    }

    static int on_header_field(http_parser* parser, const char *at, size_t length)
    {
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onHeaderField)
        {
            return _this->onHeaderField(at, length);
        }
        return 0;
    }

    static int on_header_value(http_parser* parser, const char *at, size_t length)
    {
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->on_header_value)
        {
            return _this->onHeaderValue(at, length);
        }
        return 0;
    }

    static int on_headers_complete(http_parser* parser)
    {
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onHeadersComplete)
        {
            return _this->onHeadersComplete();
        }
        return 0;
    }

    static int on_body(http_parser* parser, const char *at, size_t length)
    {
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onBody)
        {
            return _this->onBody(at, length);
        }
        return 0;
    }

    static int on_message_complete(http_parser* parser)
    {
        HttpParser* _this  = reinterpret_cast<HttpParser*>(parser->data);
        if (_this->onMessageComplete)
        {
            return _this->onMessageComplete();
        }
        return 0;
    }

};

std::unique_ptr<http_parser_settings> HttpParser::m_settings;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpParser hp;
    hp.onBody=[](const char *at, size_t length)->int
    {

        qDebug()<<QByteArray(at, length);
        return 0;
    };

    hp.onMessageComplete=[]()->int
    {

        qDebug()<<"MessageComplete";
        return 0;
    };

    QByteArray d("GET /hi HTTP/1.1\r\nContent-Length:0\r\n\r\n");
    hp.execute(d.constData(), d.length());


    return a.exec();
}
