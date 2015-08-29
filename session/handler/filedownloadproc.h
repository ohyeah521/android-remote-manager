#ifndef FILEDOWNLOADPROC_H
#define FILEDOWNLOADPROC_H

#include "../datapack.h"

class FileDownloadProc:public QObject
{
    Q_OBJECT
public:
    FileDownloadProc(DataPack* dataPack,const QString& path, int length):
        mPath(path),mLength(length),mDataPack(dataPack)
    {
        QObject::connect(this,SIGNAL(start()),this,SLOT(handleStart()));
    }
    virtual ~FileDownloadProc(){}

signals:
    void start();
    void progress(int);

private slots:
    void handleStart();

private:
    QString mPath;
    int mLength;
    DataPack* mDataPack;
};

#endif // FILEDOWNLOADPROC_H
