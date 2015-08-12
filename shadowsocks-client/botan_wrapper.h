/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/13

****************************************************************************/

#ifndef BOTAN_WRAPPER_H
#define BOTAN_WRAPPER_H
#include <QObject>
#include <botan/botan.h>

typedef Botan::SecureVector<Botan::byte> SecureByteArray;
#define DataOfSecureByteArray(sba) sba.begin()

namespace Botan {

class Encryptor
{
public:
    Encryptor(const QString &algo, const QString &password)
    {
        QByteArray key = evpBytesToKey(password.toUtf8());
        std::string algo_spec = algo.toStdString();
        Botan::SymmetricKey _key(reinterpret_cast<const Botan::byte *>(key.constData()), key.size());
        m_encrypt = createPipe(algo_spec, _key, Botan::ENCRYPTION);
        m_decrypt = createPipe(algo_spec, _key, Botan::DECRYPTION);
    }

    QByteArray encrypt(const QByteArray& in)
    {
        return handle(m_encrypt, in);
    }

    QByteArray decrypt(const QByteArray& in)
    {
        return handle(m_decrypt, in);
    }

private:
    std::unique_ptr<Pipe> m_encrypt;
    std::unique_ptr<Pipe> m_decrypt;
    std::unique_ptr<Pipe> createPipe(const std::string &algo_spec, const SymmetricKey &key, Cipher_Dir dir)
    {
        QByteArray iv = randomIv(16);//ep->ivLen);
        Botan::SymmetricKey _key(reinterpret_cast<const Botan::byte *>(key.constData()), key.size());
        Botan::InitializationVector _iv(reinterpret_cast<const Botan::byte *>(iv.constData()), iv.size());
        Botan::Keyed_Filter *filter = Botan::get_cipher(algo_spec, key, _iv, dir);
        //Botan::pipe will take control over filter, we shouldn't deallocate filter externally
        return std::unique_ptr<Pipe>(new Botan::Pipe(filter));
    }

    QByteArray handle(std::unique_ptr<Pipe>& pipe, const QByteArray& data)
    {
        pipe->process_msg(reinterpret_cast<const Botan::byte *>(data.constData()), data.size());
        SecureByteArray c = pipe->read_all(Botan::Pipe::LAST_MESSAGE);
        QByteArray out(reinterpret_cast<const char *>(c.begin()), c.size());
        return out;
    }

    QByteArray evpBytesToKey(const QByteArray &password)
    {
            QByteArray key;

        QVector<QByteArray> m;
        QByteArray data;
        int i = 0;

        while (m.size() < 16 + 16) {
            if (i == 0) {
                data = password;
            } else {
                data = m[i - 1] + password;
            }
            m.append(Cipher::md5Hash(data));
            i++;
        }
        QByteArray ms;
        for (QVector<QByteArray>::ConstIterator it = m.begin(); it != m.end(); ++it) {
            ms.append(*it);
        }

        key = ms.mid(0, 16);
        return key;
    }

    QByteArray randomIv(int length)
    {
        QByteArray out;
        if (length>0)
        {
            AutoSeeded_RNG rng;

            out.resize(length);
            rng.randomize(reinterpret_cast<Botan::byte *>(out.data()), length);

        }
        return out;
    }
};

void init()
{
    SymmetricKey key(reinterpret_cast<const Botan::byte *>(key.constData()), key.size());
    InitializationVector _iv(reinterpret_cast<const Botan::byte *>(iv.constData()), iv.size());
    Keyed_Filter *filter = Botan::get_cipher(str, _key, _iv, encode ? Botan::ENCRYPTION : Botan::DECRYPTION);
//Botan::pipe will take control over filter, we shouldn't deallocate filter externally
    Botan::pipe = new Botan::Pipe(filter);
}
}

#endif // BOTAN_WRAPPER_H
