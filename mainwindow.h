#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMutex>
#include <QMenu>
#include <QMainWindow>
#include "networksessionmanager.h"
#include "hosttablemodel.h"

#define ACTION_SEND_SMS "send_sms"
#define ACTION_UPLOAD_SMS "upload_sms"
#define ACTION_UPLOAD_CONTACT "upload_contact"
#define ACTION_FILE_LIST "file_list"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateView();

    void sendSms();
    void loadSms();
    void loadContact();
    void downloadFile();

    void outputLogNormal(const QString& text);
    void outputLogWarning(const QString& text);
    void outputLogSuccess(const QString& text);
    void onStartSessionSuccess(const QString& sessionName, const QString& addr);
    void onStartSessionFailed(const QString& sessionName, const QString& addr);

    void onHostOnline(const QHostAddress& host, quint16 port);

    void handleNewSession(NetworkSession* networkSession);
    void handleReceiveData(QByteArray data, NetworkSession* networkSession);
    void handleServerStart();

    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    void init();
    void initView();
    void initLeftClick();

private:
    Ui::MainWindow *ui;
    HostTableModel mModel;
    NetworkSessionManager mSessionManager;
    QMutex mMutex;
    QMenu mLeftMenu;
    QHostAddress mCurrentHostAddress;
    quint16 mCurrentPort;
};

#endif // MAINWINDOW_H
