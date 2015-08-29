#ifndef FILETRANSFERDIALOG_H
#define FILETRANSFERDIALOG_H

#include <QDialog>
#include <QMutexLocker>
#include "../session/session.h"

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

signals:
    void signalPutPath(QByteArray path);

public slots:
    void handleReceiveData(QByteArray data);

private slots:

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_pushButtonYes_clicked();

    void on_pushButtonUp_clicked();

private:
    Ui::FileTransferDialog *ui;
    QMutex mMutex;
    QString mSavePath;
    Session mSession;
};

#endif // FILETRANSFERDIALOG_H
