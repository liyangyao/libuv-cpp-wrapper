/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/17

****************************************************************************/

#include "mainform.h"
#include <QApplication>
#include <QDebug>
#include <Windows.h>
#include "uvpp.h"

int main(int argc, char *argv[])
{
    uvpp::Thread thread([]()
    {
        uvpp::Loop loop;
        uvpp::Tcp tcp(&loop);
        stream.read_stop();
        stream.read_start([](const QByteArray& d)
        {

        });

        uvpp::Timer timer(&loop);
        int round = 0;
        timer.start([&round, &loop]()
        {
            round++;
            qDebug()<<"In Timer at"<<round<<" threadid:"<<GetCurrentThreadId();
            if (round%2==0)
            {
                loop.queue_work([]()
                {
                    qDebug()<<"work...."<<GetCurrentThreadId();
                },[](int )
                {
                    qDebug()<<"after work...."<<GetCurrentThreadId();
                });
            }
        }, 1000, 1000);
        loop.run();

        qDebug("after run");
    });
    QApplication a(argc, argv);
    MainForm w;
    w.show();


    return a.exec();
}
