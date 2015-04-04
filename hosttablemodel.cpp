#include "hosttablemodel.h"

HostTableModel::HostTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    this->headList << "" << "IP ADDR  " << "INFO";
}

int HostTableModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int HostTableModel::columnCount(const QModelIndex &parent) const
{
    return this->headList.size();
}

QVariant HostTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() )
        return QVariant();

    if(role == Qt::TextAlignmentRole && index.column()!= 0)//设置文字对齐
    {
        return (Qt::AlignHCenter+Qt::AlignVCenter);
    }

    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 1:
            return QString();
        case 2:
            return QString();
        }
    }
    if (role == Qt::CheckStateRole && index.column()== 0)
    {
        if(this->mItemList.at(index.row()).checked)
            return Qt::Checked;
        return Qt::Unchecked;
    }
    return QVariant();
}


QVariant HostTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole
            && orientation == Qt::Horizontal //水平标题
            && section < this->headList.size()
            )
        return this->headList[section];

    return QVariant();
}

Qt::ItemFlags HostTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    if (index.column() == 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool HostTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
        QMutexLocker locker(&mMutex);
        this->mItemList.at(index.row()).checked = (value == Qt::Checked);
    }
    return true;
}

vector<HostItem> HostTableModel::getHostList()
{
    return mItemList;
}

void HostTableModel::selectAll()
{
    QMutexLocker locker(&mMutex);
    unsigned int i;
    for(i=0; i<this->mItemList.size(); ++i)
    {
        this->mItemList.at(i).checked = true;
    }
}

void HostTableModel::unselectAll()
{
    QMutexLocker locker(&mMutex);
    unsigned int i;
    for(i=0; i<this->mItemList.size(); ++i)
    {
        this->mItemList.at(i).checked = false;
    }
}

void HostTableModel::reverseSelect()
{
    QMutexLocker locker(&mMutex);
    unsigned int i;
    for(i=0; i<this->mItemList.size(); ++i)
    {
        this->mItemList.at(i).checked = !this->mItemList.at(i).checked;
    }
}

void HostTableModel::putItem(string info, string host, int port)
{
    char port_str[20] = {0};
    sprintf(port_str, "%d", port);
    string address = (host + ":" + port_str);

    QMutexLocker locker(&mMutex);

    //find item in index
    map<string, int>::iterator it = mItemIndex.find(address);
    int index = 0;
    //if it exist, update item
    if(it != mItemIndex.end())
    {
        index = it->second;
    }
    else //else push it back and create index in map
    {
        mItemIndex[address] = index = mItemList.size();
        mItemList.resize(mItemList.size() + 1);
    }
    //update access time
    HostItem& item = mItemList.at(index);
    item.checked = false;
    item.lastAccessTime = time(NULL);
    item.info = info;
    item.host = host;
    item.port = port;
    item.address = address;

}

void HostTableModel::cleanTimeoutItem(time_t timeout)
{
    time_t expiredTime = time(NULL) - timeout;

    QMutexLocker locker(&mMutex);

    vector<HostItem>::iterator it = mItemList.begin();
    while(it!=mItemList.end())
    {
        if( it->lastAccessTime < expiredTime )
        {
            map<string, int>::iterator mapIt = mItemIndex.find(it->address);
            if(mapIt != mItemIndex.end())
            {
                mItemIndex.erase(mapIt);
            }
            mItemList.erase(it);
            continue;
        }
        ++it;
    }
}

void HostTableModel::cleanAll()
{
    QMutexLocker locker(&mMutex);

    mItemList.clear();
    mItemIndex.clear();
}
