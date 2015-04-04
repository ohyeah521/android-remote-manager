#ifndef ANDROIDHOSTTABLEMODEL_H
#define ANDROIDHOSTTABLEMODEL_H

#include <QMutexLocker>
#include <time.h>
#include <QAbstractTableModel>
#include <QStringList>
#include <vector>
#include <string>
#include <map>
using std::string;
using std::vector;
using std::map;

struct HostItem
{
    string info;
    string host;
    int port;
    string address;
    time_t lastAccessTime;
    bool checked;
};

class HostTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit HostTableModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;              //重载QAbstractItemModel的rowCount函数
    int columnCount(const QModelIndex &parent = QModelIndex()) const;           //重载QAbstractItemModel的columnCount函数
    QVariant data(const QModelIndex &index, int role) const;                    //重载QAbstractItemModel的data函数
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;      //重载QAbstractItemModel的headerData函数
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    void putItem(string info, string host, int port);

    void cleanTimeoutItem(time_t timeout);

    void cleanAll();

    vector<HostItem> getHostList();

public slots:
    void selectAll();
    void unselectAll();
    void reverseSelect();

private:
    map<string, int> mItemIndex;
    vector<HostItem> mItemList;
    QStringList headList;
    QMutex mMutex;
};

#endif // ANDROIDHOSTTABLEMODEL_H
