/****************************************************************************

Creator: liyangyao@gmail.com
Date: 4/19/2015

****************************************************************************/

#include <QCoreApplication>
#include <libuv/wrapper/eventloop.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LibuvWrapper::EventLoop el;
    el.exec();

    return a.exec();
}
