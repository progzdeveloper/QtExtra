#include "qtcachingproxymodel.h"
#include <QMultiHash>

class QtCachingProxyModelPrivate
{
public:
    struct ItemData
    {
        QVariant value;
        int role;
        ItemData(const QVariant& v, int r) :
            value(v), role(r) {
        }
    };

    QHash<QPersistentModelIndex, ItemData> cache;

    inline void cacheIndex(const QModelIndex& index, const QVariant& value, int role);
    inline void revertModel(QAbstractItemModel* model);
};

inline void QtCachingProxyModelPrivate::cacheIndex(const QModelIndex &index, const QVariant &value, int role)
{
    cache.insertMulti(index, ItemData(value, role));
}

inline void QtCachingProxyModelPrivate::revertModel(QAbstractItemModel *model)
{
    for (auto it = cache.begin(); it != cache.end(); ++it) {
        model->setData(it.key(), (*it).value, (*it).role);
    }
}





QtCachingProxyModel::QtCachingProxyModel(QObject *parent) :
    QIdentityProxyModel(parent),
    d_ptr(new QtCachingProxyModelPrivate)
{
}

QtCachingProxyModel::~QtCachingProxyModel()
{
}

bool QtCachingProxyModel::uncommitted() const
{
    Q_D(const QtCachingProxyModel);
    return (!d->cache.empty());
}

int QtCachingProxyModel::cacheSize() const
{
    Q_D(const QtCachingProxyModel);
    return d->cache.size();
}

void QtCachingProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    submit();
    QIdentityProxyModel::setSourceModel(sourceModel);
}

bool QtCachingProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(QtCachingProxyModel);
    QModelIndex idx = mapToSource(index);
    QVariant val = idx.data(role);
    if (QIdentityProxyModel::setData(index, value, role)) {
        d->cacheIndex(idx, val, role);
        Q_EMIT changesCached(index, role);
        return true;
    }
    return false;
}


bool QtCachingProxyModel::submit()
{
    Q_D(QtCachingProxyModel);
    d->cache.clear();
    Q_EMIT accepted();
    return true;
}

void QtCachingProxyModel::revert()
{
    Q_D(QtCachingProxyModel);
    d->revertModel(sourceModel());
    d->cache.clear();
    Q_EMIT rejected();
}

bool QtCachingProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (uncommitted())
        return false;
    return QIdentityProxyModel::insertRows(row, count, parent);
}

bool QtCachingProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    if (uncommitted())
        return false;
    return QIdentityProxyModel::insertColumns(column, count, parent);
}

bool QtCachingProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (uncommitted())
        return false;
    return QIdentityProxyModel::removeRows(row, count, parent);
}

bool QtCachingProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    if (uncommitted())
        return false;
    return QIdentityProxyModel::removeColumns(column, count, parent);
}



