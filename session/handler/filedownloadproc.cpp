#include "filedownloadproc.h"

#include <QFile>
#include <vector>
using std::vector;

void FileDownloadProc::handleStart()
{
    QFile file(mPath);
    if(!file.open(QIODevice::WriteOnly))
    {
        mDataPack->close();
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
        mDataPack->socket()->waitForReadyRead();
        int length = mDataPack->socket()->read(buffer.data(),buffer.size());
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
    mDataPack->close();
}
