/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/24

****************************************************************************/

#include <QCoreApplication>
#include <QDebug>
#include "uvpp.h"
#include "thread.h"
#include "tcp.h"
#include <queue>
#include <windows.h>
#include "loopex.h"

void testServer()
{
    uv::Thread t([]()
    {
        uv::Loop loop;
        uv::Tcp server(&loop);
        server.bind("0.0.0.0", 8080);
        server.listen([&](int status)
        {
            if (status<0) return;
            uv::Tcp* client = new uv::Tcp(&loop);
            server.accept(client);
            client->read_start([client](const QByteArray &data)
            {
                qDebug()<<"recv:"<<data;
                client->write("I'm recved!!!", nullptr);

            });
            uv::Tcp *s = &server;
            client->setCloseCallback([client, s]()
            {
                qDebug()<<"closed";;
                delete client;
                s->close();
            });
        });
        loop.run();
    });
    //t.join();
}

void testClient()
{
    uv::Thread t([]()
    {
        uv::Loop loop;
        uv::Tcp* client = new uv::Tcp(&loop);
        client->connect("111.13.100.91", 80, [client](int status)
        {
            if (status==0)
            {
                client->write("GET / HTTP/1.1\r\nContent-Length: 0\r\n\r\n", nullptr);

                uv::Tcp* c = client;
                client->read_start([c](const QByteArray &data)
                {
                   qDebug()<<"Client Receive:"<<data;
                   delete c;
                });
            }
        });

        loop.run();
    });
    //t.join();
}

namespace WorkerTest {
uv_cond_t cond;
uv_mutex_t mutex;


int g_taskCount = 0;
struct Task
{
    uv::Functor work;
    uv::Functor done;
    Task()
    {
        g_taskCount ++;

    }
    ~Task()
    {
        g_taskCount --;
        qDebug()<<"Remain Task="<<g_taskCount;
    }
};
typedef std::unique_ptr<Task> TaskPtr;

std::queue<TaskPtr> wq;

void init_once()
{
    uv_cond_init(&cond);
    uv_mutex_init(&mutex);

}

void post(TaskPtr &task)
{
    uv_mutex_lock(&mutex);
    wq.push(std::move(task));
    uv_cond_signal(&cond);
    uv_mutex_unlock(&mutex);
}


void newWorkThread()
{
    uv::Thread t([]()
    {
        while(true)
        {
            uv_mutex_lock(&mutex);
            while(wq.empty())
            {
                uv_cond_wait(&cond, &mutex);
            }
            TaskPtr task;
            task.swap(wq.front());
            wq.pop();
            uv_mutex_unlock(&mutex);
            task->work();
        }
        //交还任务
    });
}

void setup()
{
    init_once();
    for(int i=0; i<1; i++)
    {
        newWorkThread();
    }
}

void MainTest()
{
    WorkerTest::setup();
    for(int i=1; i<=10; i++)
    {
        WorkerTest::TaskPtr t(new WorkerTest::Task);
        t->work = [i]()
        {
            ::Sleep(1000);
            qDebug()<<"run task "<<i<<"["<<GetCurrentThreadId()<<"]";
        };
        qDebug()<<"--->"<<i<<"["<<GetCurrentThreadId()<<"]";
        WorkerTest::post(t);
        ::Sleep(500);
    }
}
}

namespace vvv{
uv_async_t done_async;
void init()
{

}
}

namespace TestLoopEx {
    uv::LoopEx* g_loopEx = nullptr;
    void setup()
    {
        uv::Thread t([]()
        {
            uv::LoopEx lp;
            g_loopEx = &lp;
            lp.run();
        });
    }

    void mainTest()
    {
        TestLoopEx::setup();
        qDebug()<<"Go!";
        ::Sleep(1000);
        qDebug()<<"call queueInLoop"<<"["<<GetCurrentThreadId()<<"]";
        for(int i=0; i<5; i++)
        {
            TestLoopEx::g_loopEx->queueInLoop([]()
            {
                qDebug()<<"Yes! "<<"["<<GetCurrentThreadId()<<"]";;
            });
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TestLoopEx::mainTest();





    return a.exec();
}
