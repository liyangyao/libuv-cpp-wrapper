#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>

class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = 0);

private slots:
    int add(int a, int b);
    void print(const QString &message);
};

#endif // SERVICE_H
