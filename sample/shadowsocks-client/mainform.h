#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include <QSystemTrayIcon>

namespace Ui {
class MainForm;
}

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason);

    void on_btnActive_clicked();

private:
    Ui::MainForm *ui;
    QSystemTrayIcon *m_trayIcon;
    void changeEvent(QEvent *e);
};

#endif // MAINFORM_H
