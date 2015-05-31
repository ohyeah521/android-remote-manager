#include "networksession.h"
#include "crypt.h"

NetworkSession::NetworkSession(QAbstractSocket* socket):mSocket(socket),mHasRead(0)
{
    QObject::connect(mSocket,SIGNAL(readyRead()),this,SLOT(onReadReady()));
}

NetworkSession::~NetworkSession()
{
    if(mSocket!=NULL)
    {
        mSocket->close();
        delete mSocket;
    }
}

void NetworkSession::setReceivePackage(bool isEnable)
{
    if(isEnable)
    {
        QObject::connect(mSocket,SIGNAL(readyRead()),this,SLOT(onReadReady()));
    }
    else
    {
        QObject::disconnect(mSocket,SIGNAL(readyRead()),this,SLOT(onReadReady()));
    }
}

void NetworkSession::write(QByteArray data)
{
    if(mSocket!=NULL)
    {
        crypt().encrypt(data.data(),data.length());
        QDataStream dataStream(mSocket);
        dataStream.setByteOrder(QDataStream::BigEndian);
        dataStream << SIGNATURE;
        dataStream << data.size();
        mSocket->write(data);
        mSocket->flush();
    }
}

void NetworkSession::close()
{
    if(mSocket!=NULL)
    {
        mSocket->flush();
        mSocket->close();
    }
}

void NetworkSession::onReadReady()
{
    mSocket->waitForReadyRead(3000);
    if(mData.size()==0)
    {
        QDataStream dataStream(mSocket);
        dataStream.setByteOrder(QDataStream::BigEndian);
        qint32 signature;
        dataStream >> signature;
        if( signature != SIGNATURE )
        {
            close();
            return;
        }
        qint32 length;
        dataStream >> length;
        if( length <= 0)
        {
            close();
            return;
        }
        mData.resize(length);
        mHasRead = 0;
    }
    qint32 nRead = mSocket->read(mData.data() + mHasRead, mData.size() - mHasRead);
    if(nRead <= 0)
    {
        close();
        return;
    }
    mHasRead += nRead;
    if(mHasRead == mData.size())
    {
        crypt().decrypt(mData.data(),mData.length());
        emit onReadData(mData, this);
        mData.clear();
        mHasRead = 0;
    }
}

QAbstractSocket* NetworkSession::socket()
{
    return mSocket;
}

void NetworkSession::setSessionName(QString sessionName)
{
    mSessionName = sessionName;
}
QString NetworkSession::getSessionName()
{
    return mSessionName;
}

void NetworkSession::setSessionUuid(QString sessionUuid)
{
    mSessionUuid = sessionUuid;
}
QString NetworkSession::getSessionUuid()
{
    return mSessionUuid;
}

void NetworkSession::setSessionData(QByteArray sessionData)
{
    mSessionData = sessionData;
}
QByteArray NetworkSession::getSessionData()
{
    return mSessionData;
}
