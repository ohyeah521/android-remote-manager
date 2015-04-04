#ifndef NETWORKSERVERMANAGER_H
#define NETWORKSERVERMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpServer>
#include <vector>
using std::vector;
#include <string.h>
using std::string;

class NetworkServerManager: public QObject
{
    Q_OBJECT
public:
    NetworkServerManager();
    ~NetworkServerManager();
    bool start(int port);
    void stop();

private:
    void init();

signals:
    void onIncomeHost(QString info, QString host, quint16 port);

public slots:
    void onHostOnline();

private:
    QUdpSocket mUdpSocket;
    QTcpServer mTcpServer;
};

#endif // NETWORKSERVERMANAGER_H
