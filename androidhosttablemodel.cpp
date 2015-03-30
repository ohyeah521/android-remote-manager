#include "androidhosttablemodel.h"

AndroidHostTableModel::AndroidHostTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    this->headList << "" << "PHONE TYPE" << "VERSION" << "IP ADDR";
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
        case 3:
            return QString();
        }
    }
    if(role == Qt::TextAlignmentRole && index.column()!= 0)//设置文字对齐
    {
        return (Qt::AlignHCenter+Qt::AlignVCenter);
    }
    if (role == Qt::CheckStateRole && index.column()== 0)
    {
        if(this->m_checkedlist.at(index.row()))
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
        this->m_checkedlist.at(index.row()) = ischecked;
    }
    return true;
}

vector<bool> AndroidHostTableModel::get_checked_list()
{
    return this->m_checkedlist;
}

void AndroidHostTableModel::selectAll()
{
    unsigned int i;
    for(i=0; i<this->m_checkedlist.size(); ++i)
    {
        this->m_checkedlist.at(i) = true;
    }
}

void AndroidHostTableModel::unselectAll()
{
    unsigned int i;
    for(i=0; i<this->m_checkedlist.size(); ++i)
    {
        this->m_checkedlist.at(i) = false;
    }
}

void AndroidHostTableModel::reverseSelect()
{
    unsigned int i;
    for(i=0; i<this->m_checkedlist.size(); ++i)
    {
        this->m_checkedlist.at(i) = !this->m_checkedlist.at(i);
    }
}
