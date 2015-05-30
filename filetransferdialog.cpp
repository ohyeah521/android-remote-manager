#include "filetransferdialog.h"
#include "ui_filetransferdialog.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define ACTION_FILE_LIST "file_list"

FileTransferDialog::FileTransferDialog(NetworkSession* networkSession, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileTransferDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
    mNetworkSession = networkSession;
    QObject::connect(networkSession->socket(),SIGNAL(disconnected()),networkSession,SLOT(close()));
    QObject::connect(networkSession->socket(),SIGNAL(error(QAbstractSocket::SocketError)),networkSession,SLOT(close()));
    QObject::connect(this,SIGNAL(destroyed()),networkSession,SLOT(close()));
    QObject::connect(networkSession->socket(),SIGNAL(aboutToClose()),networkSession,SLOT(deleteLater()));
    QObject::connect(this,SIGNAL(signalPutPath(QByteArray)),networkSession,SLOT(write(QByteArray)));
    QObject::connect(networkSession,SIGNAL(onReadData(QByteArray,NetworkSession*)),this,SLOT(handleReceiveData(QByteArray)));
    QObject::connect(networkSession,SIGNAL(destroyed()),this,SLOT(close()));
    ui->setupUi(this);

    ui->lineEdit->setText("/");
}

FileTransferDialog::~FileTransferDialog()
{
    delete ui;
}

void FileTransferDialog::putPath(const QByteArray& path)
{
    QMutexLocker locker(&mMutex);
    emit signalPutPath(path);
}

void FileTransferDialog::handleReceiveData(QByteArray data)
{
    ui->listWidget->clear();
    QJsonObject dataJsonObject = QJsonDocument::fromJson(data).object();
    QJsonObject::iterator it = dataJsonObject.find(ACTION_FILE_LIST);
    if(it == dataJsonObject.end()) return;
    QJsonArray jsonArray = it.value().toArray();
    for(QJsonArray::iterator arrayIt = jsonArray.begin(); arrayIt != jsonArray.end(); ++arrayIt)
    {
        QJsonObject jsonObject = (*arrayIt).toObject();
        QString name = jsonObject.take("name").toString("");
        if(jsonObject.take("type").toInt() != 0 )
        {
            name += "/";
        }
        ui->listWidget->addItem(name);
    }
}

void FileTransferDialog::on_listWidget_doubleClicked(const QModelIndex &index)
{
    if(index.row() >= ui->listWidget->count()) return;
    QString filename = ui->listWidget->item(index.row())->text();
    if(filename.length() >=2 && filename.at(filename.length()-1) == '/')
    {
        QString path = ui->lineEdit->text();
        if(path.at(path.length()-1) != '/')
        {
            path += "/" ;
        }
        path += filename.left(filename.length() - 1);
        ui->lineEdit->setText(path);
        putPath(path.toLocal8Bit());
    }
}

void FileTransferDialog::on_pushButtonYes_clicked()
{
    putPath(ui->lineEdit->text().toLocal8Bit());
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
            putPath(path.toLocal8Bit());
            break;
        }
    }
    if (i == -1)
    {
        path = '/';
        ui->lineEdit->setText(path);
        putPath(path.toLocal8Bit());
    }
}
