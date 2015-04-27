#include "hosttablemodel.h"
#include <QJsonDocument>
#include <QJsonObject>


HostTableModel::HostTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    this->headList << "" << "IP ADDR  " << "BRAND" << "MODEL" << "SIM OPERATOR" << "IMEI" << "IMSI";
    QObject::connect(&mTimer,SIGNAL(timeout()),this,SLOT(cleanTimeoutItem()));
    mTimer.start( (mTimeout = 10000) );
    mSelectedCount = 0;
}

int HostTableModel::rowCount(const QModelIndex &parent) const
{
    return mItemList.size();
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
            return (mItemList.at(index.row())->address);
        case 2:
            return (mItemList.at(index.row())->info.brand);
        case 3:
            return (mItemList.at(index.row())->info.model);
        case 4:
            return (mItemList.at(index.row())->info.simOperator);
        case 5:
            return (mItemList.at(index.row())->info.imei);
        case 6:
            return (mItemList.at(index.row())->info.imsi);
        }
    }
    if (role == Qt::CheckStateRole && index.column()== 0)
    {
        if(this->mItemList.at(index.row())->checked)
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
    beginResetModel();
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
        QMutexLocker locker(&mMutex);
        if( (value == Qt::Checked) && !( this->mItemList.at(index.row())->checked )  )
        {
            ++mSelectedCount;
        } else if( !(value == Qt::Checked) && ( this->mItemList.at(index.row())->checked )  )
        {
            --mSelectedCount;
        }
        this->mItemList.at(index.row())->checked = (value == Qt::Checked);
    }
    endResetModel();
    return true;
}

int HostTableModel::getSelectedCount()
{
    return mSelectedCount;
}

void HostTableModel::selectAll()
{
    QMutexLocker locker(&mMutex);
    beginResetModel();
    mSelectedCount = this->mItemList.size();
    unsigned int i;
    for(i=0; i<this->mItemList.size(); ++i)
    {
        this->mItemList.at(i)->checked = true;
    }
    endResetModel();
}

void HostTableModel::unselectAll()
{
    QMutexLocker locker(&mMutex);
    beginResetModel();
    mSelectedCount = 0;
    unsigned int i;
    for(i=0; i<this->mItemList.size(); ++i)
    {
        this->mItemList.at(i)->checked = false;
    }
    endResetModel();
}

void HostTableModel::reverseSelect()
{
    QMutexLocker locker(&mMutex);
    beginResetModel();
    mSelectedCount = this->mItemList.size() - mSelectedCount;
    unsigned int i;
    for(i=0; i<this->mItemList.size(); ++i)
    {
        this->mItemList.at(i)->checked = !this->mItemList.at(i)->checked;
    }
    endResetModel();
}

time_t HostTableModel::getTimeout() const
{
    return mTimeout;
}

void HostTableModel::setTimeout(const time_t &value)
{
    mTimeout = value;
}


void HostTableModel::putItem(QString info, QHostAddress host, quint16 port)
{
    QString address = (host.toString() + ":" + QString("%1").arg(port));

    QMutexLocker locker(&mMutex);
    beginResetModel();
    //find item in index
    map<QString, HostItem*>::iterator it = mItemIndex.find(address);
    HostItem *pItem = NULL;
    //if it exist, update item
    if(it != mItemIndex.end())
    {
        pItem = it->second;
    }
    else //else push it back and create index in map
    {
        pItem = new HostItem;
        mItemList.push_back(pItem);
        mItemIndex[address] = pItem;
        pItem->checked = false;
    }
    //update access time
    pItem->lastAccessTime = (time(NULL) * 1000);
    pItem->addr.first = host;
    pItem->addr.second = port;
    pItem->address = address;

    QJsonObject jsonObject = QJsonDocument::fromJson(info.toLocal8Bit()).object();
    QJsonObject::iterator jsonIt = jsonObject.find("imei");
    if(jsonIt != jsonObject.end())
    {
        pItem->info.imei = jsonIt.value().toString();
    }
    jsonIt = jsonObject.find("imsi");
    if(jsonIt != jsonObject.end())
    {
        pItem->info.imsi = jsonIt.value().toString();
    }
    jsonIt = jsonObject.find("sim_operator");
    if(jsonIt != jsonObject.end())
    {
        pItem->info.simOperator = jsonIt.value().toString();
    }
    jsonIt = jsonObject.find("brand");
    if(jsonIt != jsonObject.end())
    {
        pItem->info.brand = jsonIt.value().toString();
    }
    jsonIt = jsonObject.find("model");
    if(jsonIt != jsonObject.end())
    {
        pItem->info.model = jsonIt.value().toString();
    }

    endResetModel();
}

void HostTableModel::cleanTimeoutItem()
{
    time_t expiredTime = (time(NULL) * 1000) - mTimeout;

    QMutexLocker locker(&mMutex);
    beginResetModel();
    vector<HostItem*>::iterator it = mItemList.begin();
    while(it!=mItemList.end())
    {
        HostItem* pItem = (*it);
        if( pItem->lastAccessTime < expiredTime )
        {
            if(pItem->checked)
            {
                -- mSelectedCount;
            }
            map<QString, HostItem*>::iterator mapIt = mItemIndex.find(pItem->address);
            if(mapIt != mItemIndex.end())
            {
                mItemIndex.erase(mapIt);
            }
            mItemList.erase(it);
            delete pItem;
            continue;
        }
        ++it;
    }
    endResetModel();
}

void HostTableModel::cleanAll()
{
    QMutexLocker locker(&mMutex);

    beginResetModel();

    mItemList.clear();
    mItemIndex.clear();

    endResetModel();
}


vector<pair<QHostAddress, quint16> > HostTableModel::getSelectedHostAddr()
{
    vector<pair<QHostAddress, quint16> > addrList;
    QMutexLocker locker(&mMutex);
    vector<HostItem*>::iterator it = mItemList.begin();
    while(it!=mItemList.end())
    {
        if((*it)->checked)
        {
            addrList.push_back((*it)->addr);
        }
        ++it;
    }
    return addrList;
}
