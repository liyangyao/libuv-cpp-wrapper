/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#include <QCoreApplication>
#include "uvpp.h"
#include <QThread>
#include "loop.h"
#include "idle.h"
#include "timer.h"
#include "tcp.h"
#include "dns.h"
#include "async.h"
#include "loopex.h"
#include "thread.h"

uv::Loop *g_loop;
class UvRunThread:public QThread
{
private:
    void onIdle(uv::Loop* loop, uv::Idle* idle)
    {
        static int round = 0;
        round ++;
        qDebug()<<"onIdle round="<<round;
        if (round==5)
        {
            idle->stop();
        }
    }

    void onTimer(uv::Loop* loop, uv::Timer* timer)
    {
        static int round = 0;
        round ++;
        qDebug()<<"onTimer round="<<round;
        if (round==1)
        {
            timer->stop();
            for(int i=0; i<2; i++)
            {
                loop->queueInLoop([i]()
                {
                    qDebug()<<"--->queueInLoop at"<<i;
                });
            }
//            {
//                uv::Async *async = new uv::Async(loop);
//                async->send(nullptr);
//                async->send([async]()
//                {
//                   qDebug()<<"Execute aysnc";
//                   delete async;
//                });
//            }
//            {
//                uv::Async *async = new uv::Async(loop);
//                async->send(nullptr);
//                async->send([async]()
//                {
//                   qDebug()<<"Execute aysnc2";
//                   delete async;
//                });
//            }
        }
    }


    void onConnect(uv::Tcp* tcp, int status)
    {
        tcp->read_start();
        tcp->write("GET / HTTP/1.1\r\nContent-Length: 0\r\n\r\n", nullptr);
    }

//    static void prepare_cb(uv_prepare_t* handle)
//    {
//        qDebug()<<"prepare_cb";
//    }


    void run()
    {
        uv::LoopEx loop;
        g_loop = &loop;
//        uv::Idle idle(&loop);
//        uv::Timer timer(&loop);
//        uv::Tcp tcp(&loop);
//        uv_prepare_t prepare;
//        uv_prepare_init(loop.handle(), &prepare);
//        uv_prepare_start(&prepare, prepare_cb);
        //tcp.connect("111.13.100.91", 80, std::bind(&UvRunThread::onConnect, this, &tcp, std::placeholders::_1));
  //      timer.start(std::bind(&UvRunThread::onTimer, this, &loop, &timer), 1000, 1000);
        //idle.start(std::bind(&UvRunThread::onIdle, this, &loop, &idle));
        //uv::Dns dns;
        //dns.resolve(&loop, "www.baidu.com", nullptr);



        loop.run();
    }
};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    UvRunThread thread;
//    thread.start();

    uv::Thread t([]
    {
        qDebug()<<"run in thread"<<GetCurrentThreadId();
        Sleep(1000);
    });
    t.join();
    qDebug()<<"after join"<<GetCurrentThreadId();


    //thread.wait();


    return a.exec();
}
