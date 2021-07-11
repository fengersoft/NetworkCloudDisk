#ifndef URLSETDIALOG_H
#define URLSETDIALOG_H

#include "../api/global/globaldata.h"
#include <QDialog>
#include <QSettings>

namespace Ui {
class UrlSetDialog;
}

class UrlSetDialog : public QDialog {
    Q_OBJECT

public:
    explicit UrlSetDialog(QWidget* parent = nullptr);
    ~UrlSetDialog();

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::UrlSetDialog* ui;
};
void getBaseUrl();
void setBaseUrl(const QString& url);

#endif // URLSETDIALOG_H
