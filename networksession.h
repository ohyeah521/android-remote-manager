#ifndef NETWORKSESSION_H
#define NETWORKSESSION_H

#include <QAbstractSocket>
#include <QDataStream>
#include <QtEndian>
#include <QHostAddress>

class NetworkSession: public QObject
{
    Q_OBJECT
public:
    static const int SIGNATURE = -4353; // 0XEEFF
    NetworkSession(QAbstractSocket* socket);
    ~NetworkSession();
    void setSessionName(QString sessionName);
    QString getSessionName();
    void setSessionUuid(QString sessionUuid);
    QString getSessionUuid();
    void setSessionData(QByteArray sessionData);
    QByteArray getSessionData();
    QAbstractSocket* socket();
    void setReceivePackage(bool isEnable);
public slots:
    void write(QByteArray data);
    void close();
private slots:
    void onReadReady();
signals:
    void onReadData(QByteArray data, NetworkSession* networkSession);
private:
    QAbstractSocket* mSocket;
    QByteArray mData;
    qint32 mHasRead;
    QString mSessionName;
    QString mSessionUuid;
    QByteArray mSessionData;
public:
    QHostAddress addr;
    quint16 port;
};


#endif // NETWORKSESSION_H
