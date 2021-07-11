#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../api/controls/setvaluedialog.h"
#include "../api/dialogs/selectfiledialog.h"
#include "../api/file/fileseacher.h"
#include "../api/global/globalconst.h"
#include "../api/global/globaldata.h"
#include "../api/http/synchttpobject.h"
#include "../api/tcp/netdiskclient.h"
#include "datastructs.h"
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QFileIconProvider>
#include <QCryptographicHash>
#include <QDesktopServices>
#include "../api/controls/logowidget.h"
#include <QHBoxLayout>

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
    void addFolder();
    QString addFolder(QString parentUuid, QString name);
    QString addFile(QString parentUuid, QString name, QFileInfo& fileInfo);
    QString getFileUserDataStr(FileUser& fileUser);
    void getAllFolders();
    void getChildFolders(QString parentId);
    void loadTvDataItems(QStandardItemModel* model, QList<FileUser>& list,
                         QStandardItem* parentItem = nullptr, QString rootId = NullUuid);
    ///获取文件的Md5
    QString getFileMd5(QString pathName);
    QString getTempFilePath(QStandardItem* item);
    void deleteFilesById(const QString& uuid);
    ///上传下载记录
    void recordUploadDownLoad(QString fileName, QString fileMd5, QString operate);
private slots:
    void on_btnUpLoad_clicked();

    void on_btnDownLoad_clicked();


    void findFileInfo(QString rootPath, QFileInfo& fileInfo);

    void on_tvLeft_clicked(const QModelIndex& index);

    void on_btnNew_clicked();

    void on_tvFiles_doubleClicked(const QModelIndex& index);

    void on_btnTransferList_clicked();

    void on_btnMyDisk_clicked();

    void on_btnDelete_clicked();

    void on_btnRefresh_clicked();





private:
    Ui::MainWindow* ui;
    NetDiskClient* m_netClient;
    QStandardItemModel* m_model;
    QStandardItemModel* m_fileModel;
    QStandardItemModel* m_transModel;
    QMap<QString, QString> m_Ids;
    QString m_parentId;
    LogoWidget* m_logoWidget;
};
#endif // MAINWINDOW_H
