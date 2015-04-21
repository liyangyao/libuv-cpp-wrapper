/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/20

****************************************************************************/

#include <QCoreApplication>
#include "uvqt.h"
#include <QThread>
#include "loop.h"
#include "idle.h"
#include "timer.h"
#include "tcp.h"

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
        if (round==5)
        {
            timer->stop();
        }
    }



    void run()
    {
        uv::Loop loop;
        uv::Idle idle(&loop);
        uv::Timer timer(&loop);
        uv::Tcp tcp(&loop);
        tcp.connect("111.13.100.91", 80, [](bool connected)
        {
            qDebug()<<"Connected:"<<connected;
        });
        timer.start(std::bind(&UvRunThread::onTimer, this, &loop, &timer), 1000, 1000);
        idle.start(std::bind(&UvRunThread::onIdle, this, &loop, &idle));
        loop.run();
    }
};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    UvRunThread thread;
    thread.start();
    thread.wait();
    qDebug()<<"thread end";

    return a.exec();
}
