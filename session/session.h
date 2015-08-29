#pragma once
#include <QAbstractSocket>
#include <QHostAddress>

#include "networkmanager.h"
#include "hostinfo.h"

class NetworkManager;

class Session
{
public:
    Session(){}
    Session(NetworkManager* networkManager, QAbstractSocket* socket, const HostInfo& hostInfo, QString sessionName, QVariant sessionData):
        networkManager(networkManager),socket(socket),hostInfo(hostInfo),sessionName(sessionName),sessionData(sessionData)
    {}

    QAbstractSocket *getSocket() const
    {
        return socket;
    }

    QString getSessionName() const
    {
        return sessionName;
    }

    QVariant getSessionData() const
    {
        return sessionData;
    }

    HostInfo getHostInfo() const
    {
        return hostInfo;
    }

    NetworkManager* getNetworkManager() const
    {
        return networkManager;
    }

private:
    QAbstractSocket *socket;
    HostInfo hostInfo;
    QString sessionName;
    QVariant sessionData;
    NetworkManager* networkManager;
};

class SessionHandler
{
public:
    virtual void handleSession(Session session)=0;
};

