/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/30

****************************************************************************/

#include <QString>
#include <QtTest>
#include <uvpp/base/HttpParser.h>

class Http_parser_test : public QObject
{
    Q_OBJECT

public:
    Http_parser_test();

private Q_SLOTS:
    void noContentLength();
    void chunked();
    void testIndex();
};

Http_parser_test::Http_parser_test()
{
}

void Http_parser_test::noContentLength()
{
    bool pass = false;
    //QByteArray d("POST /post HTTP/1.1\r\nContent-Length: 3\r\n\r\nYes");
    QByteArray d("POST /post HTTP/1.1\r\nConnection: close\r\n\r\nYes");
    HttpParser hp;
    hp.onBody = [&](const char *at, size_t length)->int
    {
        pass = true;
        qDebug()<<"body:"<<QByteArray(at, length);
        return 0;
    };
    hp.execute(d.constData(), d.length());
    hp.execute(nullptr, 0);

    QVERIFY2(pass, "Failure");
}

void Http_parser_test::chunked()
{
    bool pass = false;
    QByteArray d("POST /post HTTP/1.1\r\nTransfer-encoding: chunked\r\n\r\n"
                 "1\r\na\r\n"
                 "2\r\na");
    QByteArray d2("b\r\n"
                 "0\r\n");
    HttpParser hp;
    hp.onMessageBegin = []()->int
    {
        return 0;
    };
    hp.onHeadersComplete = []()
    {
        return 0;
    };

    hp.onBody = [&](const char *at, size_t length)->int
    {
        pass = true;
        qDebug()<<"body:"<<QByteArray(at, length);
        return 0;
    };
    hp.execute(d.constData(), d.length());
    hp.execute(d2.constData(), d2.length());
    hp.execute(nullptr, 0);

    QVERIFY2(pass, "Failure");
}

void Http_parser_test::testIndex()
{
    QByteArray d("POST /post HTTP/1.1\r\nContent-Length: 3\r\n\r\nYes");
    HttpParser hp;
    int nStart;
    hp.onMessageBegin = [&]()->int
    {
        qDebug()<<"MessageBegin"<<hp.nread();
        nStart = hp.nread();
        return 0;
    };
    hp.onHeadersComplete = [&]()->int
    {
        qDebug()<<"HeadersComplete"<<hp.nread();
        qDebug()<<QByteArray::fromRawData(d.constData() + nStart -1, hp.nread()-nStart + 1);
        return 0;
    };

    hp.onBody = [&](const char *at, size_t length)->int
    {
        qDebug()<<"body:"<<QByteArray(at, length);
        return 0;
    };
    hp.execute(d.constData(), d.length());
}

QTEST_APPLESS_MAIN(Http_parser_test)

#include "tst_http_parser_test.moc"
