#include "mainform.h"
#include "ui_mainform.h"
#include <QDebug>
#include <Windows.h>
#include <time.h>
#include <QTimer>

#pragma execution_character_set("utf-8")

MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    QIcon icon = QIcon(":/icon/lfinger0.ico");
    setWindowIcon(icon);
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(icon);
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    m_trayIcon->show();
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::onTrayIconActivated(QSystemTrayIcon::ActivationReason)
{
    setVisible(!isVisible());
    if (isVisible())
    {
        bool isIcon=IsIconic(winId());
        if(!isIcon)
        {
            SetForegroundWindow(winId());
        }
        else{
            OpenIcon(winId());
        }

    }
}

void MainForm::on_btnActive_clicked()
{
    QIcon icon = QIcon(":/icon/lfinger1.ico");
    setWindowIcon(icon);
    m_trayIcon->setIcon(icon);
    ui->btnActive->setText("停用代理");
}

void MainForm::changeEvent(QEvent *e)
{
    if((e->type()==QEvent::WindowStateChange)&&isMinimized())
    {
        QTimer::singleShot(100, this, SLOT(hide()));
    }
}

