#ifndef NETWORKSERVERMANAGER_H
#define NETWORKSERVERMANAGER_H

#include <QUuid>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QMutexLocker>
#include <time.h>
#include <map>
using std::pair;
using std::map;
#include <vector>
using std::vector;
#include <string.h>
using std::string;

#include "networksession.h"

struct SessionInfo
{
    QByteArray sessionData;
    QString sessionName;
    time_t createTime;
    QHostAddress addr;
    quint16 port;
    int status;
};

class NetworkSessionManager: public QObject
{
    Q_OBJECT
public:
    static const short SIGNATURE = -13570; // 0XCAFE
    enum {
        OPERATION_SYN = -1,
        OPERATION_ACK = -2,
        OPERATION_HEARTBEAT = 0,
        OPERATION_CONNECT_HOST = 1,
        OPERATION_LISTEN_HOST = 2,
        OPERATION_ACCEPT_HOST = 3
    };

    NetworkSessionManager();
    virtual ~NetworkSessionManager();
    bool isStart();
    bool start(int port);
    void stop();
    void startSessionOnHosts( vector<pair<QHostAddress, quint16> > addrList, const QString& sessionName, const QByteArray& sessionData = QByteArray());
    void startSessionOnHost(const QHostAddress& addr, quint16 port, const QString& sessionName, const QByteArray& sessionData = QByteArray());
    void sendSynPack(const QHostAddress& addr, quint16 port, const QByteArray& sessionUuid);

    time_t getTimeout() const;
    void setTimeout(time_t value);

private:
    void init();
    void handleTimeoutSessions();

signals:
    void onStartSessionSuccess(QString sessionName, QString addr);
    void onStartSessionFailed(QString sessionName, QString addr);
    void onIncomeHost(const QString& info, const QHostAddress& host, quint16 port);
    void onNewSession(NetworkSession* networkSession);

private slots:
    void onHostOnline();
    void onNewConnect();
    void handleNewSession(NetworkSession* networkSession, const QByteArray& data);
    void onTimeout();

private:
    time_t mTimeout;
    QUdpSocket mUdpSocket;
    QTcpServer mTcpServer;
    QMutex mMutex;
    bool mIsStart;
    QTimer mTimer;
    map<QString,SessionInfo> mSessionMap;
};

#endif // NETWORKSERVERMANAGER_H
