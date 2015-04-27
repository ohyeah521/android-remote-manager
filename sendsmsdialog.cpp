#include "sendsmsdialog.h"
#include "ui_sendsmsdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

SendSmsDialog::SendSmsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendSmsDialog)
{
    ui->setupUi(this);
    initView();
}

SendSmsDialog::~SendSmsDialog()
{
    delete ui;
}

void SendSmsDialog::initView()
{
    ui->AllContactsRadioButton->click();
    ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *aAdd, *aImport, *aClear, *aRemove;
    ui->listWidget->addAction( (aAdd = new QAction("Add", ui->listWidget)) );
    ui->listWidget->addAction( (aImport = new QAction("Import", ui->listWidget)) );
    ui->listWidget->addAction( (aRemove = new QAction("Remove", ui->listWidget)) );
    ui->listWidget->addAction( (aClear = new QAction("Clear", ui->listWidget)) );
    QObject::connect(aAdd, SIGNAL(triggered()),this, SLOT(onAdd()));
    QObject::connect(aImport, SIGNAL(triggered()),this, SLOT(onImport()));
    QObject::connect(aRemove, SIGNAL(triggered()),this, SLOT(onRemove()));
    QObject::connect(aClear, SIGNAL(triggered()),this, SLOT(onClear()));
}

QString SendSmsDialog::getContent()
{
    return ui->textEditContent->toPlainText();
}

void SendSmsDialog::on_pushButton_clicked()
{
    if(ui->NumberListRadioButton->isChecked() && ui->listWidget->count()== 0)
    {
        QMessageBox::warning(this,QString("Warning"), QString("Please add number in list"), QMessageBox::Yes);
        return;
    }
    if(getContent().length()==0)
    {
        QMessageBox::warning(this,QString("Warning"), QString("Content can not blank"), QMessageBox::Yes);
        return;
    }
    int ret = QMessageBox::warning(this,QString(""), QString("Send?"), QMessageBox::Yes, QMessageBox::No);
    if(ret == QMessageBox::Yes)
    {
        this->accept();
    }
}

void SendSmsDialog::onAdd()
{
    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle("Add");
    inputDialog.setLabelText("Please add phone number:");
    if(inputDialog.exec()==QDialog::Accepted)
    {
        ui->listWidget->addItem(inputDialog.textValue());
    }
}

void SendSmsDialog::onImport()
{
    QString path = QFileDialog::getOpenFileName(this,"import contacts file", QString(), NULL);
    if(path.size()==0) return;
    QFile file(path);
    if(file.open(QFile::ReadOnly|QFile::Text))
    {
        QByteArrayList dataList = file.readAll().split('\n');
        foreach(QByteArray data,dataList)
        {
            ui->listWidget->addItem(data);
        }
    }
}

void SendSmsDialog::onRemove()
{
    QList<QListWidgetItem*> itemList = ui->listWidget->selectedItems();
    for(int i=itemList.size()-1; i>=0; --i)
    {
        delete itemList.at(i);
    }
}

void SendSmsDialog::onClear()
{
    ui->listWidget->clear();
}

bool SendSmsDialog::isSendNumberList()
{
    return ui->NumberListRadioButton->isChecked();
}

vector<QString> SendSmsDialog::getPhoneNumberList()
{
    vector<QString> list;
    for(int i=0; i< ui->listWidget->count(); ++i)
    {
        list.push_back(ui->listWidget->item(i)->text());
    }
    return list;
}
