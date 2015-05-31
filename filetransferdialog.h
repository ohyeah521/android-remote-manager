#ifndef FILETRANSFERDIALOG_H
#define FILETRANSFERDIALOG_H

#include <QDialog>
#include <QMutexLocker>
#include "networksession.h"
#include "networksessionmanager.h"

namespace Ui {
class FileTransferDialog;
}

class FileTransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileTransferDialog(NetworkSession* networkSession, NetworkSessionManager& networkSessionManager, QWidget *parent = 0);
    ~FileTransferDialog();

    void putPath(const QString& path);
    void download(const QString& path);

signals:
    void signalPutPath(QByteArray path);

private slots:
    void handleReceiveData(QByteArray data);

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_pushButtonYes_clicked();

    void on_pushButtonUp_clicked();

private:
    Ui::FileTransferDialog *ui;
    NetworkSession *mNetworkSession;
    NetworkSessionManager& mSessionManager;
    QMutex mMutex;
    QString mSavePath;
};

#endif // FILETRANSFERDIALOG_H
