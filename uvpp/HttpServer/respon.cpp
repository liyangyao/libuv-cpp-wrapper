/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/3

****************************************************************************/

#include "respon.h"

Respon::Respon():
    m_connection(nullptr)
{
    qDebug()<<"Respon::Respon";
}

Respon::~Respon()
{
    qDebug()<<"Respon::~Respon";
}

void Respon::connectEstablished(Connection *connection)
{
    qDebug()<<"Respon::connectEstablished";
    Q_ASSERT(!m_connection);
    m_connection = connection;
}

void Respon::connectDestroyed()
{
    qDebug()<<"Respon::connectDestroyed";
    m_connection = nullptr;
}

void Respon::write(const QByteArray &d)
{
    if (!m_connection) return;
    QString head = QString("HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\r\nContent-Length: %1\r\n\r\n")
            .arg(d.size());
    QByteArray output = head.toUtf8();
    output.append(d);
    //c->sendData(output);
    m_connection->write(output, nullptr);
}

void Respon::close()
{
    if (m_connection)
    {
        m_connection->shutdown();
    }
}
