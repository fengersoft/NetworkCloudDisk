#include "urlsetdialog.h"
#include "ui_urlsetdialog.h"

UrlSetDialog::UrlSetDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::UrlSetDialog)
{
    ui->setupUi(this);
    getBaseUrl();
    ui->edtUrl->setText(GlobalData::baseUrl);
}

UrlSetDialog::~UrlSetDialog()
{
    delete ui;
}

void getBaseUrl()
{
    QSettings ini(QApplication::applicationDirPath() + "/config.ini");
    ini.beginGroup("set");
    GlobalData::baseUrl = ini.value("url", "").toString();
    ini.endGroup();
}

void setBaseUrl(const QString& url)
{
    QSettings ini(QApplication::applicationDirPath() + "/config.ini");
    ini.beginGroup("set");
    GlobalData::baseUrl = url;
    ini.setValue("url", url);
    ini.endGroup();
}

void UrlSetDialog::on_btnOk_clicked()
{
    setBaseUrl(ui->edtUrl->text());
    accept();
}

void UrlSetDialog::on_btnCancel_clicked()
{
    reject();
}
