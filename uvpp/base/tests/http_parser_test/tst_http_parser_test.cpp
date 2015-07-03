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

QTEST_APPLESS_MAIN(Http_parser_test)

#include "tst_http_parser_test.moc"
