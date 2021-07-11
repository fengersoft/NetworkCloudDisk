#include "adminmanagerwindow.h"
#include "ui_adminmanagerwindow.h"

AdminManagerWindow::AdminManagerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminManagerWindow)
{
    ui->setupUi(this);
}

AdminManagerWindow::~AdminManagerWindow()
{
    delete ui;
}
