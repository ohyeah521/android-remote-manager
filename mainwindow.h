#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include "networksessionmanager.h"
#include "hosttablemodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateView();

    void sendSms();
    void loadSms();
    void loadContact();

    void handleNewSession(NetworkSession* networkSession);

private:
    void init();
    void initView();

private:
    Ui::MainWindow *ui;
    HostTableModel mModel;
    NetworkSessionManager mSessionManager;
};

#endif // MAINWINDOW_H
