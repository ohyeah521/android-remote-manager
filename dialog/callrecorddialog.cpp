#include "callrecorddialog.h"
#include "ui_callrecorddialog.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>

#include "../session/datapack.h"
#include "../defines.h"

CallRecordDialog::CallRecordDialog(const Session &session,QWidget *parent):
    QDialog(parent),mSession(session),
    ui(new Ui::CallRecordDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* aRefresh;
    ui->listWidget->addAction(aRefresh = new QAction(QStringLiteral("刷新") ,ui->listWidget));
    QObject::connect(aRefresh,SIGNAL(triggered()),this,SLOT(on_refresh()));
}

CallRecordDialog::~CallRecordDialog()
{
    delete ui;
}

void CallRecordDialog::putPath(const QString& path)
{
    QMutexLocker locker(&mMutex);

    QJsonObject jsonObject;
    QJsonDocument jsonDocument;
    jsonObject.insert(QString("action"), QString("ls"));
    jsonObject.insert(QString("path"), path);
    jsonDocument.setObject(jsonObject);

    emit signalPutPath(jsonDocument.toJson());
}

void CallRecordDialog::on_refresh()
{
    putPath("");
}

void CallRecordDialog::handleReceiveData(QByteArray data)
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
            QString name = jsonObject.take("name").toString("");
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
            QString save_path = QFileDialog::getSaveFileName(NULL,QStringLiteral("另存为"),name);
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

void CallRecordDialog::on_listWidget_doubleClicked(const QModelIndex &index)
{
    if(index.row() >= ui->listWidget->count()) return;
    QString filename = ui->listWidget->item(index.row())->text();
    if (filename.length() >=1 )
    {
        putPath(filename);
    }
}
