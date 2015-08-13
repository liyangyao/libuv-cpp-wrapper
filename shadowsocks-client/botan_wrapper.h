/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/13

****************************************************************************/

#ifndef BOTAN_WRAPPER_H
#define BOTAN_WRAPPER_H
#include <QObject>
#include <botan/botan.h>
#include <QCryptographicHash>

namespace Botan {

class Encryptor
{
public:
    Encryptor()
    {
        //setup("AES-128/CFB", "Njg1MjgxZD", 128, 16);
        m_keyLen = 16;
        m_ivLen = 16;
    }

//    bool setup(const QString &algo, const QString &password, int keyLen, int ivLen)
//    {
//        m_algo = algo;
//        m_ivLen = ivLen;
//        m_keyLen = keyLen;

//        QByteArray key = evpBytesToKey(password.toUtf8());
//        m_key = Botan::SymmetricKey(reinterpret_cast<const Botan::byte *>(key.constData()), key.size());
//        m_encrypt.reset();
//        m_decrypt.reset();
//        return true;
//    }

    QByteArray encrypt(const QByteArray& in)
    {
        if (!m_encrypt)
        {
            InitializationVector iv(rng(), m_ivLen);
            m_encrypt.reset(new Botan::Pipe(Botan::get_cipher(algo_spec(), key(), iv, Botan::ENCRYPTION)));

            return QByteArray::fromRawData((const char *)iv.begin(), iv.length())  + handle(m_encrypt, in);
        }
        return handle(m_encrypt, in);
    }

    QByteArray decrypt(const QByteArray& in)
    {
        if (!m_decrypt)
        {

            InitializationVector iv((byte *)in.constData(), m_ivLen);

            m_decrypt.reset(new Botan::Pipe(Botan::get_cipher(algo_spec(), key(), iv, Botan::DECRYPTION)));

            return handle(m_decrypt, in.mid(m_ivLen));
        }
        return handle(m_decrypt, in);
    }


private:
    int m_ivLen;
    int m_keyLen;
//    Botan::SymmetricKey m_key;
//    QString m_algo;
    std::unique_ptr<Pipe> m_encrypt;
    std::unique_ptr<Pipe> m_decrypt;

    QByteArray handle(std::unique_ptr<Pipe>& pipe, const QByteArray& data)
    {
        pipe->process_msg(reinterpret_cast<const Botan::byte *>(data.constData()), data.size());
        Botan::SecureVector<byte> c = pipe->read_all(Botan::Pipe::LAST_MESSAGE);
        QByteArray out(reinterpret_cast<const char *>(c.begin()), c.size());
        return out;
    }

    Botan::AutoSeeded_RNG &rng()
    {
        static AutoSeeded_RNG aRng;
        return aRng;
    }

    std::string &algo_spec()
    {
        static std::string s("AES-128/CFB");
        return s;
    }

    Botan::SymmetricKey &key()
    {
        static Botan::SymmetricKey aKey;
        static bool loaded = false;
        if (!loaded)
        {
            QByteArray key = evpBytesToKey("Njg1MjgxZD");
            aKey = Botan::SymmetricKey(reinterpret_cast<const Botan::byte *>(key.constData()), key.size());
        }
        return aKey;
    }

    QByteArray evpBytesToKey(QByteArray password)
    {
        QByteArray md5;
        QByteArray output;
        while (output.length() < m_keyLen)
        {
            md5 = QCryptographicHash::hash(password, QCryptographicHash::Md5);
            output.append(md5);
            password = md5 + password;
        }
        return output.left(m_keyLen);
    }

};

}

#endif // BOTAN_WRAPPER_H
