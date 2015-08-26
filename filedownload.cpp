#include "filedownload.h"

FileDownload::FileDownload(NetworkSession* networkSession, const QString &path, int length)
{
    mNetworkSession = networkSession;
    mPath = path;
    mLength = length;
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    QObject::connect(networkSession->socket(),SIGNAL(disconnected()),networkSession,SLOT(close()));
    QObject::connect(networkSession->socket(),SIGNAL(error(QAbstractSocket::SocketError)),networkSession,SLOT(close()));
    QObject::connect(this,SIGNAL(destroyed()),networkSession,SLOT(close()));
    QObject::connect(networkSession->socket(),SIGNAL(aboutToClose()),networkSession,SLOT(deleteLater()));
    QObject::connect(networkSession,SIGNAL(destroyed()),this,SLOT(deleteLater()));
    QObject::connect(this,SIGNAL(start()),this,SLOT(handleStart()));
}

void FileDownload::handleStart()
{
    QFile file(mPath);
    if(!file.open(QIODevice::WriteOnly))
    {
        mNetworkSession->close();
        return;
    }
    vector<char> buffer;
    buffer.resize(32*1024*1024);
    int total_length = 0;
    if(mLength == 0)
    {
        emit progress(100);
    }
    while(total_length < mLength)
    {
        mNetworkSession->socket()->waitForReadyRead();
        int length = mNetworkSession->socket()->read(buffer.data(),buffer.size());
        if(length < 0 )
        {
            file.remove();
            emit progress(-1);
            break;
        }
        file.write(buffer.data(),length);
        total_length += length;
        emit progress(total_length * 100 / mLength);
    }
    file.close();
    mNetworkSession->close();
}
