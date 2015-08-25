#ifndef CALLRECORDDIALOG_H
#define CALLRECORDDIALOG_H

#include <QDialog>
#include <QMutexLocker>
#include "networksession.h"
#include "networksessionmanager.h"

namespace Ui {
class CallRecordDialog;
}

class CallRecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CallRecordDialog(NetworkSession* networkSession, NetworkSessionManager& networkSessionManager, QWidget *parent = 0);
    ~CallRecordDialog();

    void putPath(const QString& path);

signals:
    void signalPutPath(QByteArray path);

private slots:
    void handleReceiveData(QByteArray data);

    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_refresh();

private:
    Ui::CallRecordDialog *ui;
    NetworkSession *mNetworkSession;
    NetworkSessionManager& mSessionManager;
    QMutex mMutex;
    QString mSavePath;
};

#endif // CALLRECORDDIALOG_H
