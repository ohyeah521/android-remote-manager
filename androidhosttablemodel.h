#ifndef ANDROIDHOSTTABLEMODEL_H
#define ANDROIDHOSTTABLEMODEL_H

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
    string address;
    int port;
    time_t lastAccessTime;
};

class AndroidHostTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AndroidHostTableModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;              //重载QAbstractItemModel的rowCount函数
    int columnCount(const QModelIndex &parent = QModelIndex()) const;           //重载QAbstractItemModel的columnCount函数
    QVariant data(const QModelIndex &index, int role) const;                    //重载QAbstractItemModel的data函数
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;      //重载QAbstractItemModel的headerData函数
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    vector<bool> get_checked_list();

    void putItem(string info, string host, int port);

    void cleanTimeoutItem(time_t timeout);

    void cleanAll();

protected:
    void mutexLock();
    void mutexUnlock();

public slots:
    void selectAll();
    void unselectAll();
    void reverseSelect();

private:
    map<string, int> mItemIndex;
    vector<HostItem> mItemList;
    QStringList headList;
    vector<bool> mCheckedlist;
};

#endif // ANDROIDHOSTTABLEMODEL_H
