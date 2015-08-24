#include <QCoreApplication>
#include <uvpp/uvpp.h>
#include <uvpp/net/tcpserver.h>
#include <QDebug>
#include <queue>
#include <unordered_set>
#include <set>
#include <QCryptographicHash>

#include <uvpp/base/threadlocal.h>

int round = -1;
class Entry
{
public:
    explicit Entry(const uvpp::ConnectionPtr &ptr):
        m_conn(ptr)
    {

    }

    ~Entry()
    {
        qDebug()<<"Entry Died";
        round = -1;
        uvpp::ConnectionPtr conn = m_conn.lock();
        if (conn)
        {
            conn->shutdown();
        }

    }

private:
    uvpp::ConnectionWeakPtr m_conn;
};

typedef std::shared_ptr<Entry> EntryPtr;
typedef std::weak_ptr<Entry> WeakEntryPtr;


class Context
{
    WeakEntryPtr entry;
};

typedef std::set<EntryPtr> Bucket;
std::queue<Bucket> queue;


void on_new_connection(const uvpp::ConnectionPtr &conn)
{
    round = 0;
    EntryPtr entry(new Entry(conn));
    queue.back().insert(entry);
    conn->entry =  entry;
}

void on_connection_close(const uvpp::ConnectionPtr &conn)
{
    qDebug()<<"Closed";
}

void on_message(const uvpp::ConnectionPtr &conn, const QByteArray &d)
{
    round = 0;
    qDebug()<<"update";
    queue.back().insert(conn->entry.lock());
    conn->write(d);
}

//void on_timer()
//{

//}

uvpp::ThreadLocal<int> tls_int;

void run_echoserver()
{
    qDebug()<<"tls_int:"<<tls_int.value();
    uvpp::Loop loop;
    uvpp::TcpServer server(&loop);
    server.listen("0.0.0.0", 81);
    server.onNewConnection = on_new_connection;
    server.onConnectionClose = on_connection_close;
    server.onMessage = on_message;


    //init queue
    {
        Bucket bucket;
        for(int i=0; i<8; i++)
        {
            queue.push(bucket);
        }
    }

    uvpp::Timer timer(&loop);
    timer.start([]()
    {
        if (round>=0)
        {
            round++;
            qDebug()<<"round="<<round;
        }
        queue.push(Bucket());
        queue.pop();
    }, 1000, 1000);
    loop.run();
}

int main(int argc, char *argv[])
{
    tls_int.value() = 101;
    qDebug()<<"tls_int:"<<tls_int.value();
    tls_int.reset();
    qDebug()<<"tls_int:"<<tls_int.value();
    QCoreApplication a(argc, argv);
    uvpp::Thread t(run_echoserver);
    return a.exec();
}
