#ifndef ANDROIDHOSTTABLEMODEL_H
#define ANDROIDHOSTTABLEMODEL_H

#include <QMutexLocker>
#include <time.h>
#include <QAbstractTableModel>
#include <QHostAddress>
#include <QStringList>
#include <QTimer>
#include <QHostAddress>
#include <vector>
#include <map>
#include "../session/hostpool.h"
using std::vector;
using std::map;
using std::pair;

class HostTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit HostTableModel(HostPool& hostPool, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;              //重载QAbstractItemModel的rowCount函数
    int columnCount(const QModelIndex &parent = QModelIndex()) const;           //重载QAbstractItemModel的columnCount函数
    QVariant data(const QModelIndex &index, int role) const;                    //重载QAbstractItemModel的data函数
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;      //重载QAbstractItemModel的headerData函数
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

public slots:
    void refresh();
    void selectAll();
    void unselectAll();
    void reverseSelect();

private:
    HostPool& mHostPool;
    QStringList headList;
};

#endif // ANDROIDHOSTTABLEMODEL_H
