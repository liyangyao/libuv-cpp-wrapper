/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/7/22

****************************************************************************/

#ifndef MANFORM_H
#define MANFORM_H

#include <QWidget>
#include <libcurl/curlpp.h>

namespace Ui {
class ManForm;
}


class ManForm : public QWidget
{
    Q_OBJECT

public:
    explicit ManForm(QWidget *parent = 0);
    ~ManForm();

private slots:
    void on_cbMethod_currentIndexChanged(int index);
    void on_btnPerform_clicked();

private:
    Ui::ManForm *ui;
    curl::Easy m_request;
    void initRequest();
    void setInputDataVisible(bool visible);
    void onRespon(char *ptr, size_t length);
    void serialize();
    bool loadFromSerialize();
};

#endif // MANFORM_H
