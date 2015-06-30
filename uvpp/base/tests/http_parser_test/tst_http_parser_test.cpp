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
    void testCase1();
};

Http_parser_test::Http_parser_test()
{
}

void Http_parser_test::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(Http_parser_test)

#include "tst_http_parser_test.moc"
