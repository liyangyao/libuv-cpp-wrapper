/*
 * encryptor.cpp - the source file of Encryptor class
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

#include "encryptor.h"


const char * ENCRY_METHOD = "AES-128/CFB";

Encryptor::Encryptor() :
    enCipher(nullptr),
    deCipher(nullptr)
{}

void Encryptor::reset()
{
    if (enCipher) {
        delete enCipher;
        enCipher = nullptr;
    }
    if (deCipher) {
        delete deCipher;
        deCipher = nullptr;
    }
}

QByteArray Encryptor::encrypt(const QByteArray &in)
{
    QByteArray out;
    const quint8* inp = reinterpret_cast<const quint8 *>(in.constData());


    if (!enCipher) {
        QByteArray iv = Cipher::randomIv(16);//ep->ivLen);
        enCipher = new Cipher(ENCRY_METHOD/*ep->method*/, "Njg1MjgxZD"/*ep->key*/, iv, true);
        out = iv + enCipher->update(in);
    } else {
        out = enCipher->update(in);
    }

    return out;
}

QByteArray Encryptor::decrypt(const QByteArray &in)
{
    QByteArray out;
    const quint8* inp = reinterpret_cast<const quint8 *>(in.constData());


    if (!deCipher) {
        deCipher = new Cipher(ENCRY_METHOD/*ep->method*/, "Njg1MjgxZD"/*ep->key*/, in.mid(0, 16/*ep->ivLen*/), false);
        out = deCipher->update(in.mid(16/*ep->ivLen*/));
    } else {
        out = deCipher->update(in);
    }


    return out;
}

QByteArray Encryptor::encryptAll(const QByteArray &in)
{
    QByteArray out, iv;
    const quint8* inp = reinterpret_cast<const quint8 *>(in.constData());


        if (enCipher) {
            delete enCipher;
        }
        iv = Cipher::randomIv(16/*ep->ivLen*/);
        enCipher = new Cipher(ENCRY_METHOD/*ep->method*/,"Njg1MjgxZD"/*ep->key*/, iv, true);
        out = iv + enCipher->update(in);


    return out;
}

QByteArray Encryptor::decryptAll(const QByteArray &in)
{
    QByteArray out;
    const quint8* inp = reinterpret_cast<const quint8 *>(in.constData());


        if (deCipher) {
            delete deCipher;
        }
        deCipher = new Cipher(ENCRY_METHOD/*ep->method*/, "Njg1MjgxZD"/*ep->key*/, in.mid(0, 16/*ep->ivLen*/), false);
        out = deCipher->update(in.mid(16/*ep->ivLen*/));


    return out;
}

bool Encryptor::selfTest()
{
    QByteArray test("barfoo!"), test2("Hello World!"), test3("libQtShadowsocks!");
    QByteArray res  = decrypt(encrypt(test)),
            res2 = decrypt(encrypt(test2)),
            res3 = decrypt(encrypt(test3));
    reset();
    return test == res && test2 == res2 && test3 == res3;
}
