/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/18

****************************************************************************/

#include <QCoreApplication>
#include <botan/botan.h>
#include <botan/cryptobox.h>
#include <QByteArray>
#include <QDebug>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QByteArray in("HelloWorld!");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    Botan::AutoSeeded_RNG rng;
    std::string out1 = Botan::CryptoBox::encrypt(reinterpret_cast<const Botan::byte *>(in.constData()),
                                                 in.length(), "ZiMaKaiMen", rng);
    std::string out2 = Botan::CryptoBox::encrypt(reinterpret_cast<const Botan::byte *>(in.constData()),
                                                 in.length(), "ZiMaKaiMen", rng);
    qDebug()<<QString::fromStdString(out1);
    qDebug()<<QString::fromStdString(out2);
    qDebug()<<"-----------------------";
    std::string s1 = Botan::CryptoBox::decrypt(out1, "ZiMaKaiMen");
    std::string s2 = Botan::CryptoBox::decrypt(out2, "ZiMaKaiMen");
    qDebug()<<QString::fromStdString(s1);
    qDebug()<<QString::fromStdString(s2);


    return a.exec();
}
