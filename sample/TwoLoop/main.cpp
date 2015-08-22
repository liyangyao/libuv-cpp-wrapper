#include "widget.h"
#include <QApplication>
#include <uvpp/uvpp.h>
#include <Windows.h>
#include <QDebug>

using namespace uvpp;

void thread_run(const char* name, Loop* loop, AsyncFunctor* f)
{
    qDebug()<<name<<" thread start"<<GetCurrentThreadId();
    loop->run();
    qDebug()<<"thread end"<<GetCurrentThreadId();
}

void twoLoop()
{
    Loop* loop_one = new Loop;
    AsyncFunctor* f_one = new AsyncFunctor(loop_one);
    Loop* loop_two = new Loop;
    AsyncFunctor* f_two = new AsyncFunctor(loop_two);
    new Thread(std::bind(&thread_run,"one", loop_one, f_one));
    new Thread(std::bind(&thread_run,"two", loop_two, f_two));
    ::Sleep(2000);
    f_one->queue([f_two]()
    {
        qDebug()<<"one say:hello"<<GetCurrentThreadId();
        f_two->queue([]()
        {
            qDebug()<<"two say:hello"<<GetCurrentThreadId();
            //Tcp *tcp = new Tcp(&f_two)
        });
    });
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    twoLoop();

    return a.exec();
}
