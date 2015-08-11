/*
 * cipher.cpp - the source file of Cipher class
 *
 * Copyright (C) 2014-2015 Symeon Huang <hzwhuang@gmail.com>
 *
 * This file is part of the libQtShadowsocks.
 *
 * libQtShadowsocks is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * libQtShadowsocks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <botan/auto_rng.h>
#include <botan/key_filt.h>
#include <botan/lookup.h>
#include <stdexcept>
#include <QMap>
#include <QVector>
#include <QDebug>
#include "cipher.h"

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

Cipher::Cipher(const QByteArray &method, const QByteArray &password, const QByteArray &iv, bool encode) :
    pipe(nullptr)
{
    try {
        //QString m = method;
        //std::string str = m.toLower().toStdString();
        QByteArray key = evpBytesToKey(password);
        qDebug()<<"key is"<<key;
        std::string str(method.constData(), method.length());
        Botan::SymmetricKey _key(reinterpret_cast<const Botan::byte *>(key.constData()), key.size());
        Botan::InitializationVector _iv(reinterpret_cast<const Botan::byte *>(iv.constData()), iv.size());
        Botan::Keyed_Filter *filter = Botan::get_cipher(str, _key, _iv, encode ? Botan::ENCRYPTION : Botan::DECRYPTION);
        //Botan::pipe will take control over filter, we shouldn't deallocate filter externally
        pipe = new Botan::Pipe(filter);
    } catch(Botan::Exception &e) {
        qWarning("ERROR %s\n", e.what());
    }
}

Cipher::~Cipher()
{
    if (pipe)
    {
        delete pipe;
    }
}

const QMap<QByteArray, Cipher::kiLenArray> Cipher::keyIvMap = Cipher::generateKeyIvMap();

QMap<QByteArray, Cipher::kiLenArray> Cipher::generateKeyIvMap()
{
    QMap<QByteArray, kiLenArray>map;
//    map.insert("AES-128/CFB", {16, 16});
//    map.insert("AES-192/CFB", {24, 16});
//    map.insert("AES-256/CFB", {32, 16});
//    map.insert("Blowfish/CFB", {16, 8});
//    map.insert("Camellia-128/CFB", {16, 16});
//    map.insert("Camellia-192/CFB", {24, 16});
//    map.insert("Camellia-256/CFB", {32, 16});
//    map.insert("CAST-128/CFB", {16, 8});
//    map.insert("ChaCha", {32, 8});
//    map.insert("DES/CFB", {8, 8});
//    map.insert("IDEA/CFB", {16, 8});
//    map.insert("RC2/CFB", {16, 8});
//    map.insert("RC4", {16, 0});
//    map.insert("RC4-MD5", {16, 16});
//    map.insert("Salsa20", {32, 8});
//    map.insert("SEED/CFB", {16, 16});
//    map.insert("Serpent/CFB", {32, 16});//256-bit Serpent
    return map;
}

QByteArray Cipher::update(const QByteArray &data)
{
    if (pipe) {
        pipe->process_msg(reinterpret_cast<const Botan::byte *>(data.constData()), data.size());
        SecureByteArray c = pipe->read_all(Botan::Pipe::LAST_MESSAGE);
        QByteArray out(reinterpret_cast<const char *>(DataOfSecureByteArray(c)), c.size());
        return out;
    } else {
        throw std::runtime_error("Underlying ciphers are all uninitialised!");
    }
}

QByteArray Cipher::randomIv(int length)
{
    if (length == 0) {//directly return empty byte array if no need to genenrate iv
        return QByteArray();
    }

    Botan::AutoSeeded_RNG rng;
    QByteArray out;
    out.resize(length);
    rng.randomize(reinterpret_cast<Botan::byte *>(out.data()), length);
    return out;
}

bool Cipher::isSupported(const QByteArray &method)
{
#if BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(1,11,0)
    if (method.contains("ChaCha"))  return true;
#endif

    if (method.contains("RC4")) {
        return true;
    } else {
        std::string str(method.constData(), method.length());
        Botan::Keyed_Filter *filter;
        try {
            filter = Botan::get_cipher(str, Botan::ENCRYPTION);
        } catch (Botan::Exception &e) {
            qWarning("%s\n", e.what());
            return false;
        }
        delete filter;
        return true;
    }
}
