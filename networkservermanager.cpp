#include "networkservermanager.h"
#include <QDebug>

NetworkServerManager::NetworkServerManager()
{
    init();
}

NetworkServerManager::~NetworkServerManager()
{
    stop();
}

void NetworkServerManager::init()
{
    QObject::connect(&mUdpSocket,SIGNAL(readyRead()),this,SLOT(onHostOnline()),Qt::QueuedConnection);
}

bool NetworkServerManager::start(int port)
{
    mUdpSocket.bind(port);
    mUdpSocket.open(QIODevice::ReadWrite);
    mTcpServer.listen(QHostAddress::Any, port);
    return true;
}

void NetworkServerManager::stop()
{
    mUdpSocket.close();
    mTcpServer.close();
}

void NetworkServerManager::onHostOnline()
{
    QHostAddress host;
    quint16 port;
    QByteArray datagram;
    datagram.resize(mUdpSocket.pendingDatagramSize());
    mUdpSocket.readDatagram(datagram.data(),datagram.size(), &host, &port);
    emit onIncomeHost(datagram, host.toString(), port);
}
