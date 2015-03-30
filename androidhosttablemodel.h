#ifndef ANDROIDHOSTTABLEMODEL_H
#define ANDROIDHOSTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <vector>
using std::vector;

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

public slots:
    void selectAll();
    void unselectAll();
    void reverseSelect();

private:
    QStringList headList;
    vector<bool>m_checkedlist;
};

#endif // ANDROIDHOSTTABLEMODEL_H
