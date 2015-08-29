#ifndef CALLRECORDDIALOG_H
#define CALLRECORDDIALOG_H

#include <QDialog>
#include <QMutexLocker>
#include "../session/session.h"
#include "../session/networkmanager.h"

namespace Ui {
class CallRecordDialog;
}

class CallRecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CallRecordDialog(const Session &session, QWidget *parent = 0);
    ~CallRecordDialog();

    void putPath(const QString& path);

signals:
    void signalPutPath(QByteArray path);

public slots:
    void handleReceiveData(QByteArray data);

private slots:

    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_refresh();

private:
    Ui::CallRecordDialog *ui;
    QMutex mMutex;
    QString mSavePath;
    Session mSession;
};

#endif // CALLRECORDDIALOG_H
