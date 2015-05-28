#ifndef NETWORKSESSION_H
#define NETWORKSESSION_H

#include <QAbstractSocket>
#include <QDataStream>
#include <QtEndian>

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
public slots:
    void write(const QByteArray& data);
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
};


#endif // NETWORKSESSION_H
