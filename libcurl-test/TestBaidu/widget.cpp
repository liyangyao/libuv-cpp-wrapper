/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/4/7

****************************************************************************/
#pragma execution_character_set("utf-8")

#include "widget.h"
#include "ui_widget.h"
#include <winsock2.h>
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
//#include <WinSock.h>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);   
}

Widget::~Widget()
{
    delete ui;
}
