#include "mainwindow.h"

#include "../api/global/globaldata.h"
#include "system/logindialog.h"
#include <QApplication>
int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    getBaseUrl();
    if (showLoginDialog()) {
        MainWindow w;
        w.show();
        return a.exec();
    } else {
        return -1;
    }
}
