/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/22

****************************************************************************/

#include "manform.h"
#include "ui_manform.h"


ManForm::ManForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManForm)
{
    ui->setupUi(this);
    on_cbMethod_currentIndexChanged(ui->cbMethod->currentIndex());
}

ManForm::~ManForm()
{
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

void ManForm::on_btnPerform_clicked()
{
    ui->memRespon->clear();
    m_request.reset();
    initRequest();
    if (ui->cbMethod->currentIndex()==0)
    {
        m_request.setopt_httpget();
    }
    else if (ui->cbMethod->currentIndex()==1)
    {
        m_request.setopt_httppost();
        m_request.setopt_post_fields(ui->memData->toPlainText().toUtf8().constData());
    }
    m_request.setopt_url(ui->edtUrl->text().toUtf8().constData());
    if (!m_request.perform())
    {
        ui->memRespon->insertPlainText("Error:\r\n" + QString(m_request.lastError()));
    }

}

void ManForm::initRequest()
{    m_request.setopt_timeout(ui->edtTimeout->text().toInt());
     m_request.setopt_writefunction(std::bind(&ManForm::onRespon, this, std::placeholders::_1, std::placeholders::_2));

}
