#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &MainWindow::newClientConnection);

    //启动web服务
    m_manager = new TcpServerManage(1024);
    m_manager->setHttpAcceptedCallback([this](const QPointer<JQHttpServer::Session>& session)
    {
        // 回调发生在新的线程内，不是主线程，请注意线程安全
        qDebug() << Q_FUNC_INFO << session->requestUrlPath() << "," << session->requestBody() << ","
                 << session->requestUrl();

        QMap<QString, QString> querys = session->requestUrlQuery();
        QString sql;
        QSqlQuery qry;
        if (session->requestUrlPath() == "/login")
        {
            QString username = querys["username"];
            QString pwd = querys["pwd"];
            sql = QString("select count(*) from users where username='%1' and pwd='%2'").arg(username).arg(pwd);
            qDebug() << sql;
            sqliteDao()->sqliteWrapper()->select(sql, qry);
            qry.next();
            QString ret = qry.value(0).toInt() == 1 ? "true" : "false";
            session->replyText(ret);


        }
        else if (session->requestUrlPath() == "/getUid")
        {
            QString username = querys["username"];
            sql = QString("select id from users where username='%1'").arg(username);
            qDebug() << sql;
            sqliteDao()->sqliteWrapper()->select(sql, qry);
            QString ret = "-1";
            if (qry.next())
            {
                ret = qry.value(0).toString();
            }
            session->replyText(ret);


        }
        else if (session->requestUrlPath() == "/addNewFolder")
        {
            QString uid = querys["uid"];
            QString folderName = querys["folderName"];
            QString uuid = querys["uuid"];
            sql = QString("insert into project(uid,folderName,uuid) values (%1,'%2','%3')").
                  arg(uid).arg(folderName).arg(uuid);
            qDebug() << sql;
            sqliteDao()->sqliteWrapper()->execute(sql);
            session->replyText("ok");


        }
        else if (session->requestUrlPath() == "/addData")
        {
            QString parentUuid = querys["parentUuid"];
            QString uuid = querys["uuid"];
            QString name = querys["name"];
            QString isFolder = querys["isFolder"];
            QString md5 = querys["md5"];
            QString rootuuid = querys["rootuuid"];
            sql = QString("insert into data(parentUuid,uuid,name,isFolder,md5,rootuuid) values ('%1','%2','%3',%4,'%5','%6')").
                  arg(parentUuid).arg(uuid).arg(name).arg(isFolder).arg(md5).arg(rootuuid);
            qDebug() << sql;
            sqliteDao()->sqliteWrapper()->execute(sql);
            session->replyText("ok");


        }
        else if (session->requestUrlPath() == "/delFile")
        {

            QString uuid = querys["uuid"];

            sql = QString("delete from data where uuid='%1'").
                  arg(uuid);
            qDebug() << sql;
            sqliteDao()->sqliteWrapper()->execute(sql);
            session->replyText("ok");


        }
        else if (session->requestUrlPath() == "/getFolders")
        {
            QString uid = querys["uid"];

            sql = QString("select id,folderName,uuid from project where uid=%1 order by id").arg(uid);
            qDebug() << sql;
            QSqlQuery qry;
            sqliteDao()->sqliteWrapper()->select(sql, qry);
            QJsonArray arr;
            while (qry.next())
            {
                QJsonObject obj;
                obj["id"] = qry.value(0).toInt();
                obj["folderName"] = qry.value(1).toString();
                obj["uuid"] = qry.value(2).toString();
                arr.append(obj);

            }
            session->replyJsonArray(arr);


        }
        else if (session->requestUrlPath() == "/getFiles")
        {
            QString rootUuid = querys["rootUuid"];

            sql = QString("select parentUuid,uuid,name,isFolder,md5 from data where rootUuid='%1'").arg(rootUuid);
            qDebug() << sql;
            QSqlQuery qry;
            sqliteDao()->sqliteWrapper()->select(sql, qry);
            QJsonArray arr;
            while (qry.next())
            {
                QJsonObject obj;
                obj["parentUuid"] = qry.value(0).toString();
                obj["uuid"] = qry.value(1).toString();
                obj["name"] = qry.value(2).toString();
                obj["isFolder"] = qry.value(3).toInt();
                obj["md5"] = qry.value(4).toString();
                arr.append(obj);

            }
            session->replyJsonArray(arr);


        }
        else if (session->requestUrlPath() == "/getParams")
        {
            QJsonObject obj;
            obj["ip"] = ui->edtIp->text();
            obj["port"] = ui->edtPort->text().toInt();
            session->replyJsonObject(obj);

        }
        else
        {
            session->replyText("bad request");
        }
    });

    ui->setupUi(this);
    loadConfig();
}

MainWindow::~MainWindow()
{
    saveConfig();
    delete ui;
}

void MainWindow::loadConfig()
{
    QSettings ini(QApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    ini.beginGroup("set");
    ui->edtPath->setText(ini.value("path", QApplication::applicationDirPath() + "/files").toString());
    ui->edtPort->setText(ini.value("port", "9001").toString());
    ui->edtIp->setText(ini.value("ip", "127.0.0.1").toString());
    ui->edtHttpPort->setText(ini.value("port2", "9002").toString());
    ini.endGroup();
}

void MainWindow::saveConfig()
{
    QSettings ini(QApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    ini.beginGroup("set");
    ini.setValue("path", ui->edtPath->text());
    ini.setValue("port", ui->edtPort->text());
    ini.setValue("port2", ui->edtHttpPort->text());
    ini.setValue("ip", ui->edtIp->text());
    ini.endGroup();
}

QString MainWindow::getMd5FilePath(QString& md5str)
{
    int i = 0;
    QString s = "/";
    while (i < md5str.length())
    {
        s = s + md5str.mid(i, 2) + "/";
        i += 2;
    }
    return s;
}

void MainWindow::on_btnStart_clicked()
{
    m_server->listen(QHostAddress::Any, ui->edtPort->text().toInt());
    m_manager->listen(QHostAddress::Any, ui->edtHttpPort->text().toInt());
}

void MainWindow::newClientConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(onServerReadyRead()));
    ui->edtLog->append("新客户端连入");
}

void MainWindow::onServerReadyRead()
{
    ui->edtLog->append("接收数据");
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    QByteArray cmdData = socket->readLine();
    QString cmd = cmdData;
    cmd = cmd.simplified();
    ui->edtLog->append(cmd);
    if (cmd == "upload")
    {
        QByteArray data;
        do
        {
            data.append(socket->readAll());
        }
        while (socket->waitForReadyRead(10));
        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData(data);
        QString md5str = md5.result().toHex().toLower();
        ui->edtLog->append(md5str);
        QString path = getMd5FilePath(md5str);
        path = ui->edtPath->text() + path;
        QDir dir;
        dir.mkpath(path);
        QFile file(path + md5str);
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.flush();
        file.close();
    }
    else if (cmd == "download")
    {
        QByteArray md5Data = socket->readLine();
        QString md5 = md5Data;
        md5 = md5.simplified();
        QString path = getMd5FilePath(md5);

        QString filePath = ui->edtPath->text() + path + md5;
        ui->edtLog->append(filePath);
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QByteArray data = file.readAll();
        socket->write(data);
        file.close();
    }
}

void MainWindow::on_btnFolder_clicked()
{
    QString path = QFileDialog::getExistingDirectory();
    if (path == "")
    {
        return;
    }
    ui->edtPath->setText(path);
}
