#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <vector>
using namespace std;

ProgressDialog::ProgressDialog(QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    this->setWindowTitle(path);
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setProgressValue(int value)
{
    ui->progressBar->setValue(value);
    if(value<0)
    {
        QMessageBox::about(this,QString(),QStringLiteral("错误"));
        close();
    }
    else if(value >=100)
    {
        QMessageBox::about(this,QString(),QStringLiteral("成功"));
        close();
    }
}
