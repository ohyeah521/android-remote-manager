#include "hosttablemodel.h"
#include <QJsonDocument>
#include <QJsonObject>

HostTableModel::HostTableModel(HostPool& hostPool, QObject *parent) :
    mHostPool(hostPool),
    QAbstractTableModel(parent)
{
    this->headList << "" << QStringLiteral("IP地址") << QStringLiteral("上线时间") << QStringLiteral("网络状态")
                   << QStringLiteral("品牌") << QStringLiteral("版本") << QStringLiteral("手机类型")
                   << QStringLiteral("可用内存/总内存") << QStringLiteral("可用容量/总容量") << QStringLiteral("服务商")  << QStringLiteral("经度") << QStringLiteral("纬度") << "IMEI" << "IMSI";
    this->columnList << "network_state" << "brand" << "version" << "model" << "memory" << "storage" << "sim_operator" << "longitude" << "latitude" << "imei" << "imsi";
}

int HostTableModel::rowCount(const QModelIndex &parent) const
{
    return mHostPool.size();
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

    HostItem hostItem;
    if(!mHostPool.getHostItem(index.row(), hostItem))
    {
        return QVariant();
    }
    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 1:
            return (hostItem.address);
        case 2:
            return QDateTime::fromTime_t(hostItem.firstAccessTime,QTimeZone::systemTimeZone()).toString("yyyy-MM-dd HH:mm:ss");
        default:
            {
                int column = index.column()-3;
                const QStringList &list = hostItem.info.toStringList();
                if(0<=column && column<list.size() )
                {
                    return list[column];
                }
            }
        }
    }
    if (role == Qt::CheckStateRole && index.column()== 0)
    {
        if(hostItem.checked)
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
        mHostPool.checked(index.row(), value == Qt::Checked);
    }
    endResetModel();
    return true;
}

void HostTableModel::refresh()
{
    beginResetModel();
    endResetModel();
}
void HostTableModel::selectAll()
{
    beginResetModel();
    mHostPool.selectAll();
    endResetModel();
}

void HostTableModel::unselectAll()
{
    beginResetModel();
    mHostPool.unselectAll();
    endResetModel();
}

void HostTableModel::reverseSelect()
{
    beginResetModel();
    mHostPool.reverseSelect();
    endResetModel();
}
