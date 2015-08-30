#include "filedownload.h"
#include "dialog/progressdialog.h"

#include <QJsonDocument>

void FileDownload::handleSession(Session session)
{
    QJsonObject jsonObject = QJsonDocument::fromJson(session.getSessionData().toByteArray()).object();
    QString savePath = jsonObject.take("save_path").toString();
    QString path = jsonObject.take("path").toString();
    int length = jsonObject.take("length").toInt();

    DataPack* dataPack = new DataPack(session.getSocket(),false);
    FileDownloadProc* fileDownlaodProc = new FileDownloadProc(dataPack,savePath,length);
    ProgressDialog *processDialog = new ProgressDialog(path);
    QObject::connect(processDialog,SIGNAL(destroyed()),dataPack,SLOT(close()));

    QThread * thread = new QThread();
    QObject::connect(fileDownlaodProc,SIGNAL(destroyed()),thread,SLOT(quit()));
    QObject::connect(fileDownlaodProc,SIGNAL(progress(int)),processDialog,SLOT(setProgressValue(int)));
    QObject::connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    QObject::connect(thread,SIGNAL(destroyed()),processDialog,SLOT(close()));

    processDialog->show();

    fileDownlaodProc->moveToThread(thread);
    dataPack->moveToThread(thread);
    dataPack->socket()->setParent(NULL);
    dataPack->socket()->moveToThread(thread);
    thread->start();
    emit dataPack->writeDataPack(session.getSessionData().toByteArray());
    emit fileDownlaodProc->start();
}
