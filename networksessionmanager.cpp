#include "networksessionmanager.h"
#include "crypt.h"

NetworkSessionManager::NetworkSessionManager()
{
    init();
}

NetworkSessionManager::~NetworkSessionManager()
{
    stop();
}

void NetworkSessionManager::init()
{
    mIsStart = false;
    mTimeout = 10000;
    QObject::connect(&mUdpSocket,SIGNAL(readyRead()),this,SLOT(onHostOnline()));
    QObject::connect(&mTcpServer,SIGNAL(newConnection()),this,SLOT(onNewConnect()));
    QObject::connect(&mTimer,SIGNAL(timeout()),this,SLOT(onTimeout()));
}

void NetworkSessionManager::onTimeout()
{
    handleTimeoutSessions();
}

bool NetworkSessionManager::isStart()
{
    return mIsStart;
}

bool NetworkSessionManager::start(int port)
{
    if(isStart()) return false;
    mIsStart = true;
    if(!(mUdpSocket.bind(port) && mTcpServer.listen(QHostAddress::Any, port) ))
    {
        stop();
        return false;
    }
    mUdpSocket.open(QIODevice::ReadWrite);
    mTimer.start(1000);
    return true;
}

void NetworkSessionManager::stop()
{
    mIsStart = false;
    mUdpSocket.close();
    mTcpServer.close();
    mTimer.stop();

    QMutexLocker locker(&mMutex);
    mSessionMap.clear();
}

void NetworkSessionManager::onHostOnline()
{
    QHostAddress host;
    quint16 port;
    QByteArray datagram;
    datagram.resize(mUdpSocket.pendingDatagramSize());
    mUdpSocket.readDatagram(datagram.data(),datagram.size(), &host, &port);
    QDataStream dataStream(datagram);
    short signature = 0;
    dataStream >> signature;
    if( signature != SIGNATURE )
    {
        return;
    }
    short operation = 0;
    dataStream >> operation;
    if(operation == OPERATION_ACK)
    {
        QByteArray uuid(datagram);
        uuid.remove(0, sizeof(signature) + sizeof(operation) );
        QMutexLocker locker(&mMutex);
        map<QString, SessionInfo>::iterator it = mSessionMap.find(uuid);
        if(it==mSessionMap.end())
        {
            return;
        }
        if(it->second.status == OPERATION_SYN)
        {
            it->second.status = OPERATION_ACK;
        }
        else
        {
            return;
        }
        locker.unlock();

        QByteArray data;
        QDataStream sendDataStream(&data, QIODevice::WriteOnly);
        sendDataStream.setByteOrder(QDataStream::BigEndian);
        sendDataStream << SIGNATURE << (short)( OPERATION_CONNECT_HOST ); //signature & operation code
        sendDataStream.writeRawData(uuid.data(),uuid.length());
        mUdpSocket.writeDatagram(data,host,port);
    }
    else if(operation == OPERATION_HEARTBEAT)
    {
        QByteArray info(datagram);
        info.remove(0, sizeof(signature) + sizeof(operation));
        crypt().decrypt(info.data(),info.length());
        emit onIncomeHost(info, host, port);
    }
    else
    {
        ;
    }
}

void NetworkSessionManager::onNewConnect()
{
    QTcpSocket *tcpSocket = mTcpServer.nextPendingConnection();

    NetworkSession* networkSession = new NetworkSession(tcpSocket);
    QObject::connect(networkSession,SIGNAL(onReadData(QByteArray,NetworkSession*)),this,SLOT(handleNewSession(QByteArray,NetworkSession*)));
    QObject::connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),networkSession,SLOT(deleteLater()));
    QObject::connect(tcpSocket,SIGNAL(aboutToClose()),networkSession,SLOT(deleteLater()));
}

void NetworkSessionManager::handleNewSession(const QByteArray& data, NetworkSession* networkSession)
{
    QObject::disconnect(networkSession->socket(),SIGNAL(aboutToClose()),networkSession,SLOT(deleteLater()));
    QObject::disconnect(networkSession->socket(),SIGNAL(error(QAbstractSocket::SocketError)),networkSession,SLOT(deleteLater()));
    QObject::disconnect(networkSession,SIGNAL(onReadData(QByteArray,NetworkSession*)),this,SLOT(handleNewSession(QByteArray,NetworkSession*)));

    QMutexLocker locker(&mMutex);
    map<QString, SessionInfo>::iterator it = mSessionMap.find(data);
    if(it==mSessionMap.end())
    {
        networkSession->deleteLater();
        return;
    }
    emit onStartSessionSuccess(it->second.sessionName, it->second.addr.toString() + QString(":%1").arg(it->second.port));
    networkSession->addr = it->second.addr;
    networkSession->port = it->second.port;
    networkSession->setSessionName(it->second.sessionName);
    networkSession->setSessionData(it->second.sessionData);
    networkSession->setSessionUuid(it->first);
    emit onNewSession(networkSession);

    mSessionMap.erase(it);
}

void NetworkSessionManager::startSessionOnHosts( vector<pair<QHostAddress, quint16> > addrList, const QString& sessionName, const QByteArray& sessionData)
{
    if(addrList.size()==0) return;
    vector<pair<QHostAddress, quint16> >::iterator it = addrList.begin();
    while(it!=addrList.end())
    {
        startSessionOnHost(it->first,it->second,sessionName,sessionData);
        ++it;
    }
}

void NetworkSessionManager::startSessionOnHost(const QHostAddress& addr, quint16 port, const QString& sessionName, const QByteArray& sessionData)
{
    QByteArray sessionUuid = QUuid::createUuid().toByteArray();
    sendSynPack(addr,port,sessionUuid);

    SessionInfo info;
    info.sessionData = sessionData;
    info.sessionName = sessionName;
    info.createTime = (time(NULL) * 1000);
    info.status = OPERATION_SYN;
    info.addr = addr;
    info.port = port;

    QMutexLocker locker(&mMutex);
    mSessionMap[sessionUuid] = info;
}

void NetworkSessionManager::sendSynPack(const QHostAddress& addr, quint16 port, const QByteArray& sessionUuid)
{
    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::BigEndian);
    dataStream << SIGNATURE << (short)( OPERATION_SYN ); //signature & operation code
    dataStream.writeRawData(sessionUuid.data(),sessionUuid.length());
    mUdpSocket.writeDatagram(data,addr,port);
}

void NetworkSessionManager::handleTimeoutSessions()
{
    time_t expiredTime = (time(NULL) * 1000) - mTimeout;
    QMutexLocker locker(&mMutex);
    map<QString,SessionInfo>::iterator it = mSessionMap.begin();
    while(it!=mSessionMap.end())
    {
        if(it->second.createTime < expiredTime)
        {
            emit onStartSessionFailed(it->second.sessionName, it->second.addr.toString() + QString(":%1").arg(it->second.port));
            mSessionMap.erase(it);
        }
        else if (it->second.status == OPERATION_SYN)
        {
            sendSynPack(it->second.addr, it->second.port, it->first.toLocal8Bit());
        }
        ++it;
    }
}

time_t NetworkSessionManager::getTimeout() const
{
    return mTimeout;
}

void NetworkSessionManager::setTimeout(time_t value)
{
    mTimeout = value;
}
