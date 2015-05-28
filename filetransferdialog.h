#ifndef FILETRANSFERDIALOG_H
#define FILETRANSFERDIALOG_H

#include <QDialog>
#include "networksession.h"

namespace Ui {
class FileTransferDialog;
}

class FileTransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileTransferDialog(NetworkSession* networkSession, QWidget *parent = 0);
    ~FileTransferDialog();

signals:
    void putPath(QByteArray path);

private slots:
    void handleReceiveData(QByteArray data);

    void on_lineEdit_returnPressed();

private:
    Ui::FileTransferDialog *ui;
    NetworkSession *mNetworkSession;
};

#endif // FILETRANSFERDIALOG_H
