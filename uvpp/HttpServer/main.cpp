/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/3

****************************************************************************/

#include <QCoreApplication>
#include "httpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpServer s;
    s.start(80);
    return a.exec();
}
