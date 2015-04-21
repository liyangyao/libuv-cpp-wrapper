/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/18

****************************************************************************/

#include <QCoreApplication>
#include <libuv/include/uv.h>
#include <libuv/wrapper/eventloop.h>

int round = 0;

void timer_cb(uv_timer_t* handle)
{
    round++;
    printf("timer_cb=%d\n", round);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    printf("uv version is %s\n", uv_version_string());

    LibuvWrapper::EventLoop loop;
    //uv_loop_t *loop = uv_loop_new();

    uv_timer_t t;
    uv_timer_init(loop.loop(), &t);
    uv_timer_start(&t, timer_cb, 1000, 1000);
    //uv_run(loop, UV_RUN_DEFAULT);
    loop.exec();

    return a.exec();
}
