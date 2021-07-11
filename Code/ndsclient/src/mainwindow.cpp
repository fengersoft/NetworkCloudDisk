#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_netClient = new NetDiskClient(this);
    QByteArray retData;
    getHtmlData(this, GlobalData::baseUrl + "/getParams", retData);
    QJsonDocument jdoc = QJsonDocument::fromJson(retData);
    QJsonObject jobj = jdoc.object();
    m_netClient->setIp(jobj["ip"].toString());
    m_netClient->setPort(jobj["port"].toInt());


    m_model = new QStandardItemModel(this);
    ui->tvLeft->setModel(m_model);


    m_fileModel = new QStandardItemModel(this);
    ui->tvFiles->setModel(m_fileModel);
    QStringList tvFilesLabels;
    tvFilesLabels << "文件名" << "md5值";
    m_fileModel->setHorizontalHeaderLabels(tvFilesLabels);
    m_transModel = new QStandardItemModel(this);
    QStringList lvTransferLabels;
    lvTransferLabels << "文件名" << "md5值" << "操作类型" << "操作时间";
    m_transModel->setHorizontalHeaderLabels(lvTransferLabels);
    ui->lvTransfer->setModel(m_transModel);


    getAllFolders();
    showMaximized();
    m_logoWidget = new LogoWidget(ui->wgtLogo);
    QHBoxLayout* lay = new QHBoxLayout();
    ui->wgtLogo->setLayout(lay);
    lay->addWidget(m_logoWidget);
    m_logoWidget->setFilePath(QApplication::applicationDirPath() + "/data/user.jpg");
    emit ui->btnMyDisk->click();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addFolder()
{
    QString title;
    bool ret = setValue("输入文件夹名称", "文件夹名称", title);
    if (ret == false)
    {
        return;
    }
    FileUser fileUser;
    QByteArray data;
    m_parentId = GlobalData::createUuid();
    getHtmlData(this, QString(GlobalData::baseUrl + "/addNewFolder?uid=%1&folderName=%2&uuid=%3").
                arg(GlobalData::uid).arg(title).arg(m_parentId), data);
    getAllFolders();
}

QString MainWindow::addFolder(QString parentUuid, QString name)
{

    QString uuid = GlobalData::createUuid();
    QByteArray data;
    getHtmlData(this, QString(GlobalData::baseUrl +
                              "/addData?parentUuid=%1&uuid=%2&name=%3&isFolder=1&rootuuid=%4")
                .arg(parentUuid).arg(uuid).arg(name).arg(m_parentId), data);

    return uuid;
}

QString MainWindow::addFile(QString parentUuid, QString name, QFileInfo& fileInfo)
{

    QString uuid = GlobalData::createUuid();
    QByteArray data;
    QString md5 = getFileMd5(fileInfo.absoluteFilePath());
    m_netClient->uploadFile(fileInfo.absoluteFilePath(), md5);
    getHtmlData(this, QString(GlobalData::baseUrl +
                              "/addData?parentUuid=%1&uuid=%2&name=%3&isFolder=0&md5=%4&rootuuid=%5")
                .arg(parentUuid).arg(uuid).arg(name).arg(md5).arg(m_parentId), data);
    ui->lblInfo->setText("上传完成 " + fileInfo.absoluteFilePath());
    QApplication::processEvents();

    return uuid;
}

QString MainWindow::getFileUserDataStr(FileUser& fileUser)
{
    return "";
}

void MainWindow::getAllFolders()
{

    QByteArray data;
    int code = getHtmlData(this, QString(GlobalData::baseUrl + "/getFolders?uid=%1").arg(GlobalData::uid), data);



    QJsonDocument jdoc = QJsonDocument::fromJson(data);

    QJsonArray jsonMessages = jdoc.array();
    m_model->removeRows(0, m_model->rowCount());
    for (int i = 0; i < jsonMessages.count(); i++)
    {
        QJsonObject obj = jsonMessages.at(i).toObject();
        QStandardItem* item = new QStandardItem();
        item->setText(obj.value("folderName").toString());
        item->setData(obj.value("id").toInt(), Qt::UserRole + 1);
        item->setData(obj.value("uuid").toString(), Qt::UserRole + 2);
        m_model->appendRow(item);


    }


}

void MainWindow::getChildFolders(QString parentId)
{
    SyncHttpObject* http = new SyncHttpObject(this);
    http->setBaseUrl(GlobalData::baseUrl);
    QByteArray data, retData;

    getHtmlData(this, GlobalData::baseUrl + "/getFiles?rootUuid=" + parentId, data);
    QJsonDocument jdoc = QJsonDocument::fromJson(data);
    QJsonArray jsonMessages = jdoc.array();
    QList<FileUser> list;
    for (int i = 0; i < jsonMessages.count(); i++)
    {
        QJsonObject fileObj = jsonMessages.at(i).toObject();

        FileUser fileUser;
        fileUser.parentUuid = fileObj.value("parentUuid").toString();
        fileUser.uuid = fileObj.value("uuid").toString();
        fileUser.name = fileObj.value("name").toString();
        fileUser.md5 = fileObj.value("md5").toString();
        fileUser.isFolder = fileObj.value("isFolder").toInt();
        list << fileUser;
    }
    loadTvDataItems(m_fileModel, list, nullptr, parentId);
    ui->tvFiles->expandToDepth(0);
    ui->tvFiles->setColumnWidth(0, 240);
}

void MainWindow::loadTvDataItems(QStandardItemModel* model, QList<FileUser>& list, QStandardItem* parentItem, QString rootId)
{
    QStandardItem* item = nullptr;
    QStandardItem* itemMd5 = nullptr;
    QString parentId;
    if (parentItem == nullptr)
    {
        model->removeRows(0, model->rowCount());
        parentId = rootId;
        for (int i = 0; i < list.count(); i++)
        {
            FileUser fileUser = list.at(i);
            if (fileUser.parentUuid == parentId)
            {

                item = new QStandardItem();
                item->setText(fileUser.name);
                item->setData(fileUser.parentUuid, Qt::UserRole + 1);
                item->setData(fileUser.uuid, Qt::UserRole + 2);
                item->setSizeHint(QSize(200, 32));


                itemMd5 = new QStandardItem();
                itemMd5->setText(fileUser.md5);

                QFileIconProvider iconProvider;
                if (fileUser.isFolder == 0)
                {


                    item->setIcon(iconProvider.icon(QFileIconProvider::Folder));
                }
                else
                {
                    QFileInfo info(fileUser.name);
                    item->setIcon(iconProvider.icon(info));


                }

                model->appendRow(item);
                model->setItem(model->indexFromItem(item).row(), 1, itemMd5);
                loadTvDataItems(model, list, item);
            }
        }
    }
    else
    {
        parentId = parentItem->data(Qt::UserRole + 2).toString();
        for (int i = 0; i < list.count(); i++)
        {
            FileUser fileUser = list.at(i);
            if (fileUser.parentUuid == parentId)
            {

                item = new QStandardItem();
                item->setText(fileUser.name);
                item->setData(fileUser.parentUuid, Qt::UserRole + 1);
                item->setData(fileUser.uuid, Qt::UserRole + 2);
                item->setSizeHint(QSize(200, 32));
                QFileIconProvider iconProvider;
                if (fileUser.isFolder == 1)
                {


                    item->setIcon(iconProvider.icon(QFileIconProvider::Folder));
                }
                else
                {
                    QFileInfo info(fileUser.name);
                    item->setIcon(iconProvider.icon(info));


                }
                itemMd5 = new QStandardItem();
                itemMd5->setText(fileUser.md5);

                parentItem->appendRow(item);
                parentItem->setChild(model->indexFromItem(item).row(), 1, itemMd5);

                loadTvDataItems(model, list, item);
            }
        }
    }
}

QString MainWindow::getFileMd5(QString pathName)
{
    QCryptographicHash md5(QCryptographicHash::Md5);
    QFile file(pathName);
    file.open(QIODevice::ReadOnly);
    md5.addData(file.readAll());
    return md5.result().toHex().toLower();
}

QString MainWindow::getTempFilePath(QStandardItem* item)
{
    QString s = item->text();
    QStandardItem* parentItem = item->parent();
    while (parentItem != nullptr)
    {
        s = parentItem->text() + "/" + s;
        parentItem = parentItem->parent();
    }
    s = QApplication::applicationDirPath() + "/temp/" + m_parentId + "/" + s;
    return s;
}

void MainWindow::deleteFilesById(const QString& uuid)
{
    QByteArray data;
    getHtmlData(this, GlobalData::baseUrl + "/delFile?uuid=" + uuid, data);
    emit ui->tvLeft->clicked(ui->tvLeft->currentIndex());
}

void MainWindow::recordUploadDownLoad(QString fileName, QString fileMd5, QString operate)
{
    SyncHttpObject* http = new SyncHttpObject(this);
    http->setBaseUrl(GlobalData::baseUrl);
    QByteArray data, ret;
    QJsonObject jobj;
    jobj.insert("title", fileName);
    jobj.insert("hashKey", fileMd5);
    jobj.insert("operType", operate);
    jobj.insert("userName", GlobalData::uid);
    jobj.insert("u_CreateDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    jobj.insert("u_Creator", GlobalData::uid);
    QJsonDocument jdoc;
    jdoc.setObject(jobj);
    data = jdoc.toJson();
    http->postData("/FileManage/UsersLog/Insert", data, ret);
    QString s = data;
    qDebug() << s;
    delete http;
}

void MainWindow::on_btnUpLoad_clicked()
{
    SelectFileDialog* dlg = new SelectFileDialog();
    int ret = dlg->exec();
    if (ret == QDialog::Accepted)
    {
        QString rootPath = dlg->getFileInfo().absoluteFilePath();
        QFileInfo fileInfo(rootPath);
        if (fileInfo.isDir())
        {
            FileSeacher* fileSearcher = new FileSeacher(this);
            connect(fileSearcher, &FileSeacher::findFileInfo, this, &MainWindow::findFileInfo);
            m_Ids.clear();
            fileSearcher->startSearch(rootPath);
            delete fileSearcher;
        }
        else
        {

            addFile(m_parentId, fileInfo.fileName(), fileInfo);

        }
        emit ui->tvLeft->clicked(ui->tvLeft->currentIndex());

    }
    delete dlg;
}

void MainWindow::on_btnDownLoad_clicked()
{

}



void MainWindow::findFileInfo(QString rootPath, QFileInfo& fileInfo)
{
    QString id;
    if (fileInfo.isDir())
    {
        if (!m_Ids.contains(fileInfo.absolutePath()))
        {
            QFileInfo info(fileInfo.absolutePath());
            id = addFolder(m_parentId, info.fileName());
            m_Ids.insert(fileInfo.absolutePath(), id);
        }
        QString parentId = m_Ids[fileInfo.absolutePath()];
        id = addFolder(parentId, fileInfo.fileName());
        m_Ids.insert(fileInfo.absoluteFilePath(), id);

    }
    else
    {
        if (!m_Ids.contains(fileInfo.absolutePath()))
        {
            QFileInfo info(fileInfo.absolutePath());
            id = addFolder(m_parentId, info.fileName());
            m_Ids.insert(fileInfo.absolutePath(), id);
        }
        QString parentId = m_Ids[fileInfo.absolutePath()];
        id = addFile(parentId, fileInfo.fileName(), fileInfo);
        m_Ids.insert(fileInfo.absoluteFilePath(), id);
    }
}

void MainWindow::on_tvLeft_clicked(const QModelIndex& index)
{
    QStandardItem* item = m_model->itemFromIndex(index);
    m_parentId = item->data(Qt::UserRole + 2).toString();
    getChildFolders(m_parentId);
}

void MainWindow::on_btnNew_clicked()
{

    addFolder();
}

void MainWindow::on_tvFiles_doubleClicked(const QModelIndex& index)
{
    QStandardItem* item = m_fileModel->itemFromIndex(index);
    int row = item->row();
    QStandardItem* parentItem = item->parent();
    QStandardItem* itemName = nullptr;
    QString name, md5;
    if (parentItem == nullptr)
    {
        itemName = m_fileModel->item(row, 0);
        name = itemName->text();
        md5 = m_fileModel->item(row, 1)->text();

    }
    else
    {
        itemName = parentItem->child(row, 0);
        name = itemName->text();
        md5 = parentItem->child(row, 1)->text();

    }
    if (md5 != "")
    {
        QString fileName = getTempFilePath(itemName);
        QFileInfo fileInfo(fileName);
        QDir dir;
        dir.mkpath(fileInfo.absolutePath());
        qDebug() << fileInfo.absolutePath();
        m_netClient->downloadFileByMd5(md5, fileName);
        recordUploadDownLoad(name, md5, "下载");
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));

    }

}

void MainWindow::on_btnTransferList_clicked()
{
    ui->wgtPages->setCurrentIndex(1);
    ui->btnMyDisk->setChecked(false);
    ui->btnTransferList->setChecked(true);
}

void MainWindow::on_btnMyDisk_clicked()
{
    ui->wgtPages->setCurrentIndex(0);
    ui->btnMyDisk->setChecked(true);
    ui->btnTransferList->setChecked(false);
}


void MainWindow::on_btnDelete_clicked()
{
    QModelIndex index = ui->tvFiles->currentIndex();
    if (!index.isValid())
    {
        return;
    }
    int ret = QMessageBox::question(this, "提示", "是否删除选中项目?", "是", "否");
    if (ret == -1)
    {
        return;
    }

    int row = index.row();

    QStandardItem* item = m_fileModel->itemFromIndex(index);
    if (item->parent() != nullptr)
    {
        item = item->parent()->child(row);
    }
    else
    {
        item = m_fileModel->item(row);
    }
    deleteFilesById(item->data(Qt::UserRole + 2).toString());
}

void MainWindow::on_btnRefresh_clicked()
{
    emit ui->tvLeft->clicked(ui->tvLeft->currentIndex());
}




