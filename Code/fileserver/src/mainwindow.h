#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCryptographicHash>
#include <QFileDialog>
#include <QMainWindow>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <JQHttpServer.h>
#include <QSqlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include "../api/sql/sqlitedao.h"
using namespace JQHttpServer;
QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void loadConfig();
    void saveConfig();
    QString getMd5FilePath(QString& md5str);

private slots:
    void on_btnStart_clicked();
    void newClientConnection();
    void onServerReadyRead();

    void on_btnFolder_clicked();

private:
    Ui::MainWindow* ui;
    QTcpServer* m_server;
    TcpServerManage* m_manager;

};
#endif // MAINWINDOW_H
