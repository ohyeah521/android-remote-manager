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
    QObject::connect(networkSession,SIGNAL(onReadData(QByteArray,NetworkSession*)),this,SLOT(handleReceiveData(QByteArray)));
    QObject::connect(networkSession,SIGNAL(destroyed()),this,SLOT(close()));
    QObject::connect(this,SIGNAL(putPath(QByteArray)),networkSession,SLOT(write(QByteArray)));
    ui->setupUi(this);
}

FileTransferDialog::~FileTransferDialog()
{
    delete ui;
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

void FileTransferDialog::on_lineEdit_returnPressed()
{
    putPath(ui->lineEdit->text().toLocal8Bit());
}
