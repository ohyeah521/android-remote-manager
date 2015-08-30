#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include <QMenu>
#include <QTimer>
#include "session/networkmanager.h"
#include "model/hosttablemodel.h"
#include "defines.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SessionManager& sessionManager, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateHostListView();

    void sendSms();
    void loadSms();
    void loadContact();
    void listFile();
    void listAudioRecord();

    void handleServerStart();

protected:
    void closeEvent(QCloseEvent * event);

private slots:
    void outputLogNormal(const QString& text);
    void outputLogWarning(const QString& text);
    void outputLogSuccess(const QString& text);
    void onStartSessionSuccess(QString sessionName, HostInfo hostInfo);
    void onStartSessionFailed(QString sessionName, HostInfo hostInfo);
    void onHostIncome(const HostInfo& hostInfo);

    void on_tableViewHostList_doubleClicked(const QModelIndex &index);

    void on_tableViewHostList_customContextMenuRequested(const QPoint &pos);

private:
    void init();
    void initHostList();
    void initNetworkManager();
    void initMenuWithItem(QWidget* widget);
    void initMenu(QWidget* widget);

private:
    Ui::MainWindow *ui;
    HostTableModel mModelHostList;
    SessionManager& mSessionManager;
    NetworkManager mNetworkManager;
    HostInfo mHostInfo;
    int mPort;
    QMenu mMenu,mMenuWithItem;
};

#endif // MAINWINDOW_H
