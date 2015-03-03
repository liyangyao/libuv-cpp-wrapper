#include "widget.h"
#include <QApplication>
#include <QDebug>
#include <QThread>
#include "uv.h"
#include "eventloop.h"
#include "tcpserver.h"
#include "echoserver.h"
#include "chargenserver.h"

/*
    {
        std::shared_ptr<void> guard;
        std::weak_ptr<void> tie;


        {

            Obj::Ptr p(new Obj);
            tie = p;
            guard = tie.lock();
            qDebug()<<"END_1";
        }
        qDebug()<<"END_2";

    }
    qDebug()<<"END_3";
*/


int64_t counter = 0;

void wait_for_a_while(uv_idle_t* handle) {
    counter++;

    qDebug("%d", counter);
    if (counter >= 10)
        uv_idle_stop(handle);
}

class Obj
{
public:
    typedef std::shared_ptr<Obj> Ptr;
    Obj()
    {
        qDebug()<<"Obj";
    }
    ~Obj()
    {
        qDebug()<<"~Obj";
    }
};

class RunThread:public QThread
{
private:
    void run()
    {
        EventLoop loop;
        EchoServer es(&loop, 80);
        ChargenServer ss(&loop, 81);
//        TcpServer svr(&loop);
//        svr.setMessageCallback([](const TcpConnectionPtr& conn, const QByteArray& data)
//        {
//            qDebug()<<"Recv: "<<data;
//            conn->send(data);

//            //conn->shutdown();
//            //qDebug()<<"Send Shutdown";
//        });
//        svr.setCloseCallback([](const TcpConnectionPtr&)
//        {
//            qDebug()<<"Disconnected!!!";
//        });
//        qDebug()<<"listen:"<< svr.listen(80);
        loop.loop();
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;



//    {
//    EventLoop loop;
//    uv_idle_t idler;

//        uv_idle_init(loop.handle(), &idler);
//        uv_idle_start(&idler, wait_for_a_while);

//        qDebug("Idling...");

//    loop.loop();
//    }

    RunThread* t = new RunThread;
    t->start();


    w.show();

    return a.exec();
}
