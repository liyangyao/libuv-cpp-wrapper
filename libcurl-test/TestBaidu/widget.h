/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/7

****************************************************************************/
#pragma execution_character_set("utf-8")

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
