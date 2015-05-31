#ifndef FILEDOWNLOAD_H
#define FILEDOWNLOAD_H

#include "networksession.h"
#include <QString>
#include <QFile>
#include <vector>
using namespace std;

class FileDownload:public QObject
{
    Q_OBJECT
public:
    FileDownload(NetworkSession* networkSession, const QString &path, int length);
signals:
    void start();
    void progress(int value);
private slots:
    void handleStart();

private:
    NetworkSession* mNetworkSession;
    QString mPath;
    int mLength;
};

#endif // FILEDOWNLOAD_H
