#ifndef FILETRANSFERDIALOG_H
#define FILETRANSFERDIALOG_H

#include <QMenu>
#include <QDialog>
#include <QMutexLocker>
#include <QStandardItemModel>
#include "../session/session.h"
#include "filetreeitem.h"

namespace Ui {
class FileTransferDialog;
}

class FileTransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileTransferDialog(const Session& session,QWidget *parent = 0);
    ~FileTransferDialog();

    void putPath(const QString& path);

private:
    void initView();
    QString getPath(const QModelIndex &index);
    FileTreeItem* getTreeItem(const QString& path);

signals:
    void signalPutPath(QByteArray path);

public slots:
    void handleReceiveData(QByteArray data);

private slots:
    void on_treeView_clicked(const QModelIndex &index);
    void load(const QModelIndex &index);
    void refresh();

    void on_treeView_customContextMenuRequested(const QPoint &pos);

private:
    FileTreeItem* mFileManager;
    QStandardItemModel mModel;
    Ui::FileTransferDialog *ui;
    QMutex mMutex;
    QString mSavePath;
    Session mSession;
    QMenu mMenuWithIndex;
    QModelIndex mCurrentIndex;
};

#endif // FILETRANSFERDIALOG_H
