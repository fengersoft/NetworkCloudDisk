#ifndef ADMINMANAGERWINDOW_H
#define ADMINMANAGERWINDOW_H

#include <QMainWindow>

namespace Ui {
class AdminManagerWindow;
}

class AdminManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminManagerWindow(QWidget *parent = nullptr);
    ~AdminManagerWindow();

private:
    Ui::AdminManagerWindow *ui;
};

#endif // ADMINMANAGERWINDOW_H
