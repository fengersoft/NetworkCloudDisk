#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

bool LoginDialog::checkLogin(QString userName, QString password)
{


    QByteArray ret;
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(password.toLocal8Bit());
    QString md5pwd = md5.result().toHex().toLower();
    int code;
    SyncHttpObject* http;
    http = new SyncHttpObject(this);
    code = http->getHtmlData(GlobalData::baseUrl + "/login?username=" + userName + "&password=" + md5pwd, ret);
    GlobalData::userName = ui->edtUserName->text();
    QString s = ret;
    qDebug() << s;
    delete http;

    http = new SyncHttpObject(this);
    code = http->getHtmlData(GlobalData::baseUrl + "/getUid?username=" + userName, ret);
    GlobalData::uid = ret;
    delete http;
    return true;
}

bool showLoginDialog()
{
    LoginDialog* dlg = new LoginDialog();
    int ret = dlg->exec();
    if (ret == QDialog::Accepted)
    {
        ret = true;
    }
    else
    {
        ret = false;
    }
    delete dlg;
    return ret;
}

void LoginDialog::on_btnSet_clicked()
{
    UrlSetDialog* dlg = new UrlSetDialog();
    dlg->exec();
    delete dlg;
}

void LoginDialog::on_btnOk_clicked()
{
    checkLogin(ui->edtUserName->text(), ui->edtPassword->text());
    accept();
}

void LoginDialog::on_btnCancel_clicked()
{
    reject();
}
