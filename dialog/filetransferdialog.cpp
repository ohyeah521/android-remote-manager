#include "filetransferdialog.h"
#include "ui_filetransferdialog.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>

#include "../defines.h"

FileTransferDialog::FileTransferDialog(const Session &session, QWidget *parent) :
    QDialog(parent),mSession(session),
    ui(new Ui::FileTransferDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->lineEdit->setText("/");
}

FileTransferDialog::~FileTransferDialog()
{
    delete ui;
}

void FileTransferDialog::putPath(const QString& path)
{
    QMutexLocker locker(&mMutex);

    QJsonObject jsonObject;
    QJsonDocument jsonDocument;
    jsonObject.insert(QString("action"), QString("ls"));
    jsonObject.insert(QString("path"), path);
    jsonDocument.setObject(jsonObject);

    emit signalPutPath(jsonDocument.toJson());
}

void FileTransferDialog::handleReceiveData(QByteArray data)
{
    QJsonObject dataJsonObject = QJsonDocument::fromJson(data).object();
    QString action = dataJsonObject.take("action").toString();
    if(action == "ls")
    {
        ui->listWidget->clear();
        QJsonObject::iterator it = dataJsonObject.find("file_list");
        if(it == dataJsonObject.end()) return;
        QJsonArray jsonArray = it.value().toArray();
        for(QJsonArray::iterator arrayIt = jsonArray.begin(); arrayIt != jsonArray.end(); ++arrayIt)
        {
            QJsonObject jsonObject = (*arrayIt).toObject();
            QString name = jsonObject.take("name").toString();
            if(jsonObject.take("type").toInt() != 0 )
            {
                name += "/";
            }
            ui->listWidget->addItem(name);
        }
    }
    else if(action == "file")
    {
        int length = dataJsonObject.take("length").toInt();
        QString path = dataJsonObject.take("path").toString();
        QString name = dataJsonObject.take("name").toString();
        if(QMessageBox::information(this,QStringLiteral("下载文件?"),QStringLiteral("大小为 %1 Bytes").arg(length),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
        {
            QString save_path = QFileDialog::getSaveFileName(NULL,QStringLiteral("保存为"),name);
            if(save_path.length() == 0 )
            {
                return;
            }
            QJsonObject jsonObject;
            QJsonDocument jsonDocument;
            jsonObject.insert(QString("action"), QString(ACTION_FILE_DOWNLOAD));
            jsonObject.insert(QString("path"), path);
            jsonObject.insert(QString("save_path"),save_path);
            jsonObject.insert(QString("length"),length);
            jsonDocument.setObject(jsonObject);
            mSession.getNetworkManager()->startSession(mSession.getHostInfo(), ACTION_FILE_DOWNLOAD, jsonDocument.toJson());
        }
    }
}

void FileTransferDialog::on_listWidget_doubleClicked(const QModelIndex &index)
{
    if(index.row() >= ui->listWidget->count()) return;
    QString filename = ui->listWidget->item(index.row())->text();
    if(filename.length() >=2 && filename.at(filename.length()-1) == '/')
    {
        QString path = ui->lineEdit->text();
        if(path.length() > 0 && path.at(path.length()-1) != '/')
        {
            path += "/" ;
        }
        path += filename.left(filename.length() - 1);
        ui->lineEdit->setText(path);
        putPath(path);
    }
    else if (filename.length() >=1 )
    {
        QString path = ui->lineEdit->text();
        if(path.length() > 0 && path.at(path.length()-1) != '/')
        {
            path += "/" ;
        }
        path += filename;
        putPath(path);
    }
}

void FileTransferDialog::on_pushButtonYes_clicked()
{
    putPath(ui->lineEdit->text());
}

void FileTransferDialog::on_pushButtonUp_clicked()
{
    QString path = ui->lineEdit->text();
    int i,sign;
    for(i = path.length() - 1,sign = 0; i >=0 ;-- i)
    {
        if(sign == 0 && path.at(i) == '/')
        {
            sign = 1;
        }
        else if(sign == 1 && path.at(i) != '/')
        {
            path = path.left(i+1);
            ui->lineEdit->setText(path);
            putPath(path);
            break;
        }
    }
    if (i == -1)
    {
        path = '/';
        ui->lineEdit->setText(path);
        putPath(path);
    }
}
