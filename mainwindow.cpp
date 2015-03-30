#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initView();
}

MainWindow::~MainWindow()
{
    if(mpModel != NULL)
    {
        delete mpModel;
    }
    delete ui;
}

void MainWindow::initView()
{
    mpModel = new AndroidHostTableModel();
    ui->tableView->setModel(mpModel);
    ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tableView->resizeColumnToContents(0);
}
