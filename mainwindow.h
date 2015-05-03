#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include "networksessionmanager.h"
#include "hosttablemodel.h"

#define ACTION_SEND_SMS "send_sms"
#define ACTION_UPLOAD_SMS "upload_sms"
#define ACTION_UPLOAD_CONTACT "upload_contact"

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

    void outputLogNormal(const QString& text);
    void outputLogWarning(const QString& text);
    void outputLogSuccess(const QString& text);
    void onStartSessionSuccess(const QString& sessionName, const QString& addr);
    void onStartSessionFailed(const QString& sessionName, const QString& addr);

    void handleNewSession(NetworkSession* networkSession);
    void handleReceiveData(NetworkSession* networkSession, QByteArray data);
    void handleServerStart();

private:
    void init();
    void initView();

private:
    Ui::MainWindow *ui;
    HostTableModel mModel;
    NetworkSessionManager mSessionManager;
};

#endif // MAINWINDOW_H
