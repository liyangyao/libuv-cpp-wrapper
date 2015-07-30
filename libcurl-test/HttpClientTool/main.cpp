/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/22

****************************************************************************/

#include "manform.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QApplication a(argc, argv);
    ManForm w;
    w.show();

    return a.exec();
}
