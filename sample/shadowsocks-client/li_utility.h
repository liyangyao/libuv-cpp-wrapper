#ifndef LI_UTILITY_H
#define LI_UTILITY_H

#include <Windows.h>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QStringList>
#include <QVector>

namespace Utility{

extern QString getModuleName();
extern QString getHostName();
extern void installMsgHandler(bool isOutputConsole = true, bool isOutputFile = false);
extern void installMsgHandlerByFile();
extern void installMiniDump();

namespace OutputDebug{
class Thread : private QThread
{
    Q_OBJECT
public:
    explicit Thread(QObject *parent = 0);
    void installRun(bool isOutputConsole, bool isOutputFile);

private:
    struct DebugMessgage
    {
        SYSTEMTIME time;
        DWORD threadId;
        const char* level;
        QString msg;
    };

    QVector<DebugMessgage*> m_input;
    QVector<DebugMessgage*> m_output;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_isOutputConsole;
    bool m_isOutputFile;
    int m_lineCount;
    QString generateFileName();
    void queue(DebugMessgage* msg);
    void run();
    static void our_message_handler(QtMsgType type, const char * msg);
};
}//namespace OutputDebug
}//namespace Utility

#endif // LI_UTILITY_H
