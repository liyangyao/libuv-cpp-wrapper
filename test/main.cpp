/****************************************************************************

Creator: liyangyao@gmail.com
Date: 4/19/2015

****************************************************************************/

#include <QCoreApplication>
#include "eventloop.h"
#include "timer.h"
#include <QDebug>
#include <memory>
#include "Handle.h"


//libuv::EventLoop el;




//libuv::Timer *timer;
libuv::Handle<uv_timer_t> *atimer;

void timer_cb(uv_timer_t* handle)
{
    static int round = 0;
    round++;
    qDebug()<<"Timer callback "<<round;
    if (round==3)
    {
        //el.quit();
        //uv_timer_stop(handle);
        //uv_close(reinterpret_cast<uv_handle_t *>(handle), nullptr);
        //delete handle;
        delete atimer;
        uv_loop_close(uv_default_loop());
        qDebug()<<"loop close";
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    {

        atimer = new  Handle<uv_timer_t>;


        uv_timer_init(uv_default_loop(), atimer->data());
        uv_timer_start(atimer->data(), timer_cb, 1000, 1000);
        uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    //    timer = new libuv::Timer(&el);
    //    timer->start([]{
    //        static int round = 0;
    //        round++;
    //        qDebug()<<"Timer callback "<<round;
    //    }, 1000, 1000);

    //    qDebug()<<"Start";
    //    el.exec();
    }
    qDebug()<<"loop quit";

    return a.exec();
}
