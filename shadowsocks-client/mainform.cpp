#include "mainform.h"
#include "ui_mainform.h"
#include <QDebug>
#include <Windows.h>
#include <time.h>

MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
    m_secret = 100;
    m_name = "HElloaaa";
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::on_pushButton_clicked()
{
    qDebug()<<m_name;
    m_secret++;
    int sum = 0;
    for(int i=0; i<10; i++)
    {
        sum += i;
        if (sum<1000 && i==9)
        {
            _asm int 3
        }
    }
    qDebug()<<"sum="<<sum;
}

void MainForm::on_pushButton_2_clicked()
{
//    wchar_t szProgramPath[MAX_PATH] = {0};
//    if(GetModuleFileName(NULL, szProgramPath, MAX_PATH))
//    {
//           LPTSTR lpSlash = wcsrchr(szProgramPath, '\\');
//           if(lpSlash)
//           {
//               *(lpSlash + 1) = '\0';
//           }
//    }
//    wchar_t szDumpFile[MAX_PATH] = {0};
//    swprintf(szDumpFile, L"%s%d.dmp", szProgramPath, time(NULL));

    throw std::exception("A My Exception");

    int j = 0;
    int n =  10 / j;
    qDebug()<<"n="<<n;
}

void MainForm::on_pushButton_3_clicked()
{
    QList<int> list;
    list<<1<<2<<3;
    qDebug()<<list[104];
    qDebug()<< list.at(303);
}
