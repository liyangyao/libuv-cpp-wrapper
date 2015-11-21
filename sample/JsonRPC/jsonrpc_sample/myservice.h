/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/11/21

****************************************************************************/

#ifndef MYSERVICE_H
#define MYSERVICE_H

#include <QObject>
#include <QVariant>

class MyService : public QObject
{
    Q_OBJECT
public:
    explicit MyService(QObject *parent = 0);

signals:

public slots:
    int add(int a, int b);
    int add_by_object(QVariantMap map);
    QString cat(const QStringList &list);
    QString cat2(const QVariantList &list);
    void notify(const QString &msg);
};

#endif // MYSERVICE_H
