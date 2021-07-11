#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "../api/http/synchttpobject.h"
#include "urlsetdialog.h"
#include <QCryptographicHash>
#include <QDialog>

namespace Ui
{
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog();
    bool checkLogin(QString userName, QString password);

private slots:
    void on_btnSet_clicked();

    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::LoginDialog* ui;
};
bool showLoginDialog();
#endif // LOGINDIALOG_H
