#ifndef HOSTPOOL_H
#define HOSTPOOL_H

#include <QMutexLocker>
#include <time.h>
#include <QAbstractTableModel>
#include <QHostAddress>
#include <QStringList>
#include <QTimer>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>
#include <vector>
#include <map>
using std::vector;
using std::map;
using std::pair;

#include "../session/hostinfo.h"

struct HostItem
{
    QVariant info;
    HostInfo hostInfo;
    QString address;
    time_t firstAccessTime;
    time_t lastAccessTime;
    bool checked;
};

class HostPool
{
public:
    HostPool();

    time_t getTimeout() const;
    void setTimeout(const time_t &value);
    vector<HostInfo > getSelectedHostAddr();
    bool getHostItem(int index, HostItem& item);
    bool getHostInfo(int index, HostInfo& info);
    int getSelectedCount();
    unsigned int size();
    void checked(int index, bool isChecked);

    void putItem(const HostInfo& hostInfo);
    void cleanTimeoutItem();
    void cleanAll();

public:
    void selectAll();
    void unselectAll();
    void reverseSelect();

private:
    time_t currentTime();

private:
    map<QString, HostItem*> mItemIndex;
    vector<HostItem*> mItemList;
    QMutex mMutex;
    time_t mTimeout;
    int mSelectedCount;
};

#endif // HOSTPOOL_H
