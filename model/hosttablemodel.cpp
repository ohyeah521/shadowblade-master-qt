#include "hosttablemodel.h"

HostTableModel::HostTableModel(HostPool& hostPool, QObject *parent) :
    mHostPool(hostPool),
    QAbstractTableModel(parent)
{
    this->headList << "" << "IP ADDR  " << "INFO";
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
            return (hostItem.info);
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
