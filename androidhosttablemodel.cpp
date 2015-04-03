#include "androidhosttablemodel.h"

AndroidHostTableModel::AndroidHostTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    this->headList << "" << "IP ADDR  " << "INFO";
}

int AndroidHostTableModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int AndroidHostTableModel::columnCount(const QModelIndex &parent) const
{
    return this->headList.size();
}

QVariant AndroidHostTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() )
        return QVariant();

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
    if(role == Qt::TextAlignmentRole && index.column()!= 0)//设置文字对齐
    {
        return (Qt::AlignHCenter+Qt::AlignVCenter);
    }
    if (role == Qt::CheckStateRole && index.column()== 0)
    {
        if(this->mCheckedlist.at(index.row()))
            return Qt::Checked;
        return Qt::Unchecked;
    }
    return QVariant();
}


QVariant AndroidHostTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole
            && orientation == Qt::Horizontal //水平标题
            && section < this->headList.size()
            )
        return this->headList[section];

    return QVariant();
}

Qt::ItemFlags AndroidHostTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    if (index.column() == 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool AndroidHostTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
        bool ischecked = false;
        if (value == Qt::Checked) //
        {
            ischecked = true;
        }
        this->mCheckedlist.at(index.row()) = ischecked;
    }
    return true;
}

vector<bool> AndroidHostTableModel::get_checked_list()
{
    return this->mCheckedlist;
}

void AndroidHostTableModel::selectAll()
{
    unsigned int i;
    for(i=0; i<this->mCheckedlist.size(); ++i)
    {
        this->mCheckedlist.at(i) = true;
    }
}

void AndroidHostTableModel::unselectAll()
{
    unsigned int i;
    for(i=0; i<this->mCheckedlist.size(); ++i)
    {
        this->mCheckedlist.at(i) = false;
    }
}

void AndroidHostTableModel::reverseSelect()
{
    unsigned int i;
    for(i=0; i<this->mCheckedlist.size(); ++i)
    {
        this->mCheckedlist.at(i) = !this->mCheckedlist.at(i);
    }
}

void AndroidHostTableModel::mutexUnlock()
{
    //TODO
}

void AndroidHostTableModel::mutexLock()
{
    //TODO
}

void AndroidHostTableModel::putItem(string info, string host, int port)
{
    char port_str[20] = {0};
    sprintf(port_str, "%d", port);
    string address = (host + ":" + port_str);

    mutexLock();

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
    item.lastAccessTime = time(NULL);
    item.info = info;
    item.host = host;
    item.port = port;
    item.address = address;

    mutexUnlock();
}

void AndroidHostTableModel::cleanTimeoutItem(time_t timeout)
{
    time_t expiredTime = time(NULL) - timeout;

    mutexLock();

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

    mutexUnlock();
}

void AndroidHostTableModel::cleanAll()
{
    mutexLock();

    mItemList.clear();
    mItemIndex.clear();

    mutexUnlock();
}
