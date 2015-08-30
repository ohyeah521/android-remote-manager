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
    initView();

    setAttribute(Qt::WA_DeleteOnClose);
}

FileTransferDialog::~FileTransferDialog()
{
    delete ui;
}

void FileTransferDialog::initView()
{
    QStandardItem* parentItem = mModel.invisibleRootItem();
    mFileManager = new FileTreeItem("文件管理模块");
    mFileManager->setEditable(false);
    parentItem->appendRow(mFileManager);

    FileTreeItem* root = new FileTreeItem("手机内存");
    root->setEditable(false);
    root->fileName = "<ROOT>";
    mFileManager->appendRow(root);

    FileTreeItem* sd = new FileTreeItem("SD卡");
    sd->setEditable(false);
    sd->fileName = "<SD>";
    mFileManager->appendRow(sd);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView->setModel(&mModel);
    ui->treeView->expand(mFileManager->index());

    QAction *aRefresh;
    mMenuWithIndex.addAction(aRefresh = new QAction(QStringLiteral("刷新"),&mMenuWithIndex));
    QObject::connect(aRefresh,SIGNAL(triggered(bool)),this,SLOT(refresh()));
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
    QString path = dataJsonObject.take("path").toString();
    if(action == "ls")
    {
        FileTreeItem *item = this->getTreeItem(path);
        item->removeRows(0,item->rowCount());
        QJsonObject::iterator it = dataJsonObject.find("file_list");
        if(it == dataJsonObject.end()) return;
        QJsonArray jsonArray = it.value().toArray();
        for(QJsonArray::iterator arrayIt = jsonArray.begin(); arrayIt != jsonArray.end(); ++arrayIt)
        {
            QJsonObject jsonObject = (*arrayIt).toObject();
            QString name = jsonObject.take("name").toString();
//            if(jsonObject.take("type").toInt() != 0 )
//            {
//                name += "/";
//            }
            FileTreeItem * file = new FileTreeItem(name);
            file->setEditable(false);
            file->fileName = name;
            item->appendRow(file);
        }
        ui->treeView->expand(item->index());
    }
    else if(action == "file")
    {
        int length = dataJsonObject.take("length").toInt();
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

void FileTransferDialog::on_treeView_clicked(const QModelIndex &index)
{
    FileTreeItem *parentItem = static_cast<FileTreeItem*>(index.internalPointer());
    if(parentItem == NULL) return;
    FileTreeItem *item = static_cast<FileTreeItem*>(parentItem->child(index.row()));
    if(item->rowCount()>0) return;

    QString path = getPath(index);
    if(path.length()>0)
    {
        putPath(path);
    }
}

void FileTransferDialog::refresh()
{
    load(mCurrentIndex);
}

void FileTransferDialog::load(const QModelIndex &index)
{
    QString path = getPath(index);
    if(path.length()>0)
    {
        putPath(path);
    }
}

QString FileTransferDialog::getPath(const QModelIndex &index)
{
    FileTreeItem *parentItem = static_cast<FileTreeItem*>(index.internalPointer());
    if(parentItem == NULL) return QString();

    FileTreeItem *item = static_cast<FileTreeItem*>(parentItem->child(index.row()));

    //it is root, or parent is root
    if(item->fileName.length()==0 || parentItem->fileName.length()==0) return item->fileName;

    return getPath(index.parent()) + "/" + item->fileName;
}

FileTreeItem* FileTransferDialog::getTreeItem(const QString& path)
{
    FileTreeItem * item = mFileManager;
    QStringList pathList = path.split("/");
    for(int i =0; i<pathList.size(); ++i)
    {
        const QString& dir = pathList.at(i);
        FileTreeItem * findItem = NULL;
        for(int ii = 0; ii<item->rowCount(); ++ii)
        {
            findItem = static_cast<FileTreeItem*>(item->child(ii));
            if(findItem->fileName == dir)
            {
                break;
            }
        }
        if(findItem == NULL)
        {
            findItem = new FileTreeItem(dir);
            item->appendRow(findItem);
        }
        item = findItem;
    }
    return item;
}

void FileTransferDialog::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    mCurrentIndex = ui->treeView->indexAt(pos);
    if(mCurrentIndex.isValid())
    {
        mMenuWithIndex.popup(QCursor::pos());
    }
}
