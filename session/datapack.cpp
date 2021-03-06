#include "datapack.h"
#include "util/crypt.h"

DataPack::DataPack(QAbstractSocket* socket, bool asyncRead):mSocket(socket),mHasRead(0),mAsyncRead(asyncRead)
{
    if(asyncRead)
    {
        QObject::connect(mSocket,SIGNAL(readyRead()),this,SLOT(onReadReady()));
    }
}

DataPack::~DataPack()
{
}

void DataPack::writeDataPack(const char *data, qint64 len)
{
    QDataStream dataStream(mSocket);
    dataStream.setByteOrder(QDataStream::BigEndian);
    dataStream << SIGNATURE;
    dataStream << len;
    QByteArray byteArray(data,len);
    crypt().encrypt(byteArray.data(),byteArray.length());
    mSocket->write(byteArray);
    mSocket->flush();
}

void DataPack::writeDataPack(const QByteArray& data)
{
    writeDataPack(data,data.length());
}

QByteArray DataPack::readDataPack()
{
    while(mAsyncRead)
    {
        mSocket->waitForReadyRead();
        if( readingDataPack() !=0 )
        {
            break;
        }
    }
    return mData;
}

/**
 * @brief DataPack::readingDataPack
 * @return -1: read error, 0: reading, 1: read complete
 */
int DataPack::readingDataPack()
{
    //clear data
    if(mData.size() == mHasRead && mHasRead > 0)
    {
        mData.clear();
        mHasRead = 0;
    }
    //alloc data space
    if(mData.size()==0)
    {
        QDataStream dataStream(mSocket);
        dataStream.setByteOrder(QDataStream::BigEndian);
        unsigned short signature;
        dataStream >> signature;
        if( signature != SIGNATURE )
        {
            mSocket->close();
            return -1;
        }
        qint64 length;
        dataStream >> length;
        if( length <= 0)
        {
            mSocket->close();
            return -1;
        }
        mData.resize(length);
        mHasRead = 0;
    }
    // read data
    qint32 nRead = mSocket->read(mData.data() + mHasRead, mData.size() - mHasRead);
    if(nRead <= 0)
    {
        mSocket->close();
        return -1;
    }
    mHasRead += nRead;

    //read complete
    if(mHasRead == mData.size())
    {
        crypt().decrypt(mData.data(),mData.length());
        emit onReadData(mData, this);
        return 1;
    }
    return 0;
}

void DataPack::onReadReady()
{
    mSocket->waitForReadyRead(3000);
    readingDataPack();
}

QAbstractSocket* DataPack::socket()
{
    return mSocket;
}

void DataPack::close()
{
    mSocket->close();
}
