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
        setup("AES-128/CFB", "Njg1MjgxZD", 16, 16);
    }

    bool setup(const QString &algo, const QString &password, int keyLen, int ivLen)
    {
        m_algo = algo;
        m_ivLen = ivLen;
        m_keyLen = keyLen;

        QByteArray key = evpBytesToKey(password.toUtf8());
        m_key = Botan::SymmetricKey(reinterpret_cast<const Botan::byte *>(key.constData()), key.size());
        m_encrypt.reset();
        m_decrypt.reset();
        return true;
    }

    QByteArray encrypt(const QByteArray& in)
    {
        if (!m_encrypt)
        {
            std::string algo = m_algo.toStdString();
            AutoSeeded_RNG rng;
            InitializationVector iv(rng, m_ivLen);
            m_encrypt.reset(new Botan::Pipe(Botan::get_cipher(algo, m_key, iv, Botan::ENCRYPTION)));

            return QByteArray::fromRawData((const char *)iv.begin(), iv.length())  + handle(m_encrypt, in);
        }
        return handle(m_encrypt, in);
    }

    QByteArray decrypt(const QByteArray& in)
    {
        if (!m_decrypt)
        {
            std::string algo = m_algo.toStdString();
            InitializationVector iv((byte *)in.constData(), m_ivLen);

            m_decrypt.reset(new Botan::Pipe(Botan::get_cipher(algo, m_key, iv, Botan::DECRYPTION)));

            return handle(m_decrypt, in.mid(m_ivLen));
        }
        return handle(m_decrypt, in);
    }


private:
    int m_ivLen;
    int m_keyLen;
    Botan::SymmetricKey m_key;
    QString m_algo;
    std::unique_ptr<Pipe> m_encrypt;
    std::unique_ptr<Pipe> m_decrypt;

    QByteArray handle(std::unique_ptr<Pipe>& pipe, const QByteArray& data)
    {
        pipe->process_msg(reinterpret_cast<const Botan::byte *>(data.constData()), data.size());
        Botan::SecureVector<byte> c = pipe->read_all(Botan::Pipe::LAST_MESSAGE);
        QByteArray out(reinterpret_cast<const char *>(c.begin()), c.size());
        return out;
    }

    QByteArray evpBytesToKey(const QByteArray &password)
    {
            QByteArray key;

        QVector<QByteArray> m;
        QByteArray data;
        int i = 0;

        while (m.size() < m_keyLen + m_ivLen) {
            if (i == 0) {
                data = password;
            } else {
                data = m[i - 1] + password;
            }
            m.append(QCryptographicHash::hash(data, QCryptographicHash::Md5));
            i++;
        }
        QByteArray ms;
        for (QVector<QByteArray>::ConstIterator it = m.begin(); it != m.end(); ++it) {
            ms.append(*it);
        }

        key = ms.mid(0, m_keyLen);
        return key;
    }

};

}

#endif // BOTAN_WRAPPER_H
