#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include "networkservermanager.h"
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

protected:
    void initView();

private:
    Ui::MainWindow *ui;
    HostTableModel mModel;
    NetworkServerManager mServerManager;
    QThread mThread;
};

#endif // MAINWINDOW_H
