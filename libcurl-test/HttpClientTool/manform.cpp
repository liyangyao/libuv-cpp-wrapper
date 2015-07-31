/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/22

****************************************************************************/

#include "manform.h"
#include "ui_manform.h"
#include <QTextStream>
#include <QDebug>

ManForm::ManForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManForm)
{
    ui->setupUi(this);
    loadFromSerialize();
    on_cbMethod_currentIndexChanged(ui->cbMethod->currentIndex());
}

ManForm::~ManForm()
{
    serialize();
    delete ui;
}

void ManForm::on_cbMethod_currentIndexChanged(int index)
{
    setInputDataVisible(index>0);
}

void ManForm::setInputDataVisible(bool visible)
{
    ui->lblData->setVisible(visible);
    ui->memData->setVisible(visible);
}

void ManForm::onRespon(char *ptr, size_t length)
{
    ui->memRespon->insertPlainText(QString::fromUtf8(ptr, length));
}

void ManForm::serialize()
{
    QFile f(qApp->applicationDirPath()+"/session.dat");
    f.open(QFile::WriteOnly);

    int method = ui->cbMethod->currentIndex();
    int timeout = ui->edtTimeout->text().toInt();
    QString url = ui->edtUrl->text();
    QByteArray request = ui->memData->toPlainText().toUtf8();


    QTextStream(&f)<<method<<"\r\n"
                   <<timeout<<"\r\n"
                   <<url.toUtf8()<<"\r\n"
                   <<request.length()<<"\r\n"<<request;

}

bool ManForm::loadFromSerialize()
{
    QFile f(qApp->applicationDirPath()+"/session.dat");
    if (!f.open(QFile::ReadOnly))
    {
        return false;
    }
    QByteArray line = f.readLine();
    line.chop(2);
    int method = line.toInt();

    line = f.readLine();
    line.chop(2);
    int timeout = line.toInt();

    line = f.readLine();
    line.chop(2);
    QString url = QString::fromUtf8(line);

    line = f.readLine();
    line.chop(2);
    int requestLen = line.toInt();
    QByteArray c = f.read(requestLen);
    QString request = QString::fromUtf8(c);

    ui->cbMethod->setCurrentIndex(method);
    ui->edtTimeout->setText(QString::number(timeout));
    ui->edtUrl->setText(url);
    ui->memData->insertPlainText(request);
    return true;
}

void ManForm::on_btnPerform_clicked()
{
    ui->memRespon->clear();
    m_request.reset();
    initRequest();
    QByteArray body;
    if (ui->cbMethod->currentIndex()==0)
    {
        m_request.setopt_httpget();
    }
    else if (ui->cbMethod->currentIndex()==1)
    {
        m_request.setopt_httppost();
        body = ui->memData->toPlainText().toUtf8();
        m_request.setopt_post_fields(body.constData());
        m_request.setopt_post_field_size(body.size());
    }
    QByteArray url = ui->edtUrl->text().toUtf8();
    m_request.setopt_url(url.constData());
    if (!m_request.perform())
    {
        ui->memRespon->insertPlainText("Error:\r\n" + QString(m_request.lastError()));
    }

}

void ManForm::initRequest()
{
    m_request.setopt_timeout(ui->edtTimeout->text().toInt());
    m_request.setopt_writefunction(std::bind(&ManForm::onRespon, this, std::placeholders::_1, std::placeholders::_2));
}
