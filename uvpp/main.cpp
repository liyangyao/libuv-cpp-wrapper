/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#include "mainform.h"
#include <QApplication>
#include "uvpp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainForm w;
    w.show();

    uvpp::Loop loop;
    uvpp::Stream<uv_stream_t> stream;
    return a.exec();
}
