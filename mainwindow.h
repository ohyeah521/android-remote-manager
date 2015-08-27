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
#define ACTION_FILE_DOWNLOAD "file_download"
#define ACTION_CALL_RECORD "call_record"

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
    void listFile();
    void listAudioRecord();

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

    void on_tableView_customContextMenuRequested(const QPoint &pos);

private:
    void init();
    void initView();
    void initMenuWithItem(QWidget* widget);
    void initMenu(QWidget* widget);

private:
    Ui::MainWindow *ui;
    HostTableModel mModel;
    NetworkSessionManager mSessionManager;
    QMutex mMutex;
    QMenu mRightMenu;
    QMenu mRightMenuWithItem;
    QHostAddress mCurrentHostAddress;
    quint16 mCurrentPort;
};

#endif // MAINWINDOW_H
