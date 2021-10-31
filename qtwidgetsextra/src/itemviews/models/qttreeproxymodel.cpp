#include <vector>
#include <QPointer>

#include "qttreeproxymodel.h"

namespace
{

struct Group
{
    QVariant key;
    int childCount;

    Group(const QVariant& k)  : key(k), childCount(1) {}

    bool operator ==(const QVariant& value) {
        return key == value;
    }

    bool operator !=(const QVariant& value) {
        return key != value;
    }
};

}

class QtTreeProxyModelPrivate
{
public:
    QPointer<QAbstractItemModel> model;
    std::vector<Group> groups;
    int groupColumn, groupRole;

    QtTreeProxyModelPrivate()
        : model(0)
        , groupColumn(0)
        , groupRole(Qt::DisplayRole)
    {}

    int sourceRowForGroup(const QVariant& key, int expectedSourceRow) const
    {
        // Scan the source model and find row number in it
        // that corresponds with parent group key
        for (int i = 0, k = 0; i < model->rowCount(); ++i)
        {
            const QVariant value = model->index(i, groupColumn).data(groupRole);
            if (value == key)
            {
                if (k == expectedSourceRow)
                    return i;
                ++k;
            }
        }
        return -1;
    }
};


QtTreeProxyModel::QtTreeProxyModel(QObject* parent)
    : QAbstractItemModel(parent)
    , d_ptr(new QtTreeProxyModelPrivate)
{
}

QtTreeProxyModel::~QtTreeProxyModel()
{
}

void QtTreeProxyModel::setSourceModel(QAbstractItemModel *model)
{
    Q_D(QtTreeProxyModel);
    if (d->model == model)
        return;

    beginResetModel();

    if (d->model)
    {
        disconnect(d->model, &QAbstractItemModel::rowsInserted, this, &QtTreeProxyModel::onSourceRowsInserted);
        disconnect(d->model, &QAbstractItemModel::rowsRemoved, this, &QtTreeProxyModel::onSourceRowsRemoved);
        disconnect(d->model, &QAbstractItemModel::dataChanged, this, &QtTreeProxyModel::onSourceDataChanged);
        disconnect(d->model, &QAbstractItemModel::destroyed, this, &QtTreeProxyModel::onSourceDestroyed);
    }

    d->model = model;

    if (d->model)
    {
        connect(d->model, &QAbstractItemModel::rowsInserted, this, &QtTreeProxyModel::onSourceRowsInserted);
        connect(d->model, &QAbstractItemModel::rowsRemoved, this, &QtTreeProxyModel::onSourceRowsRemoved);
        connect(d->model, &QAbstractItemModel::dataChanged, this, &QtTreeProxyModel::onSourceDataChanged);
        connect(d->model, &QAbstractItemModel::destroyed, this, &QtTreeProxyModel::onSourceDestroyed);
    }

    onSourceReset(); // update our groups

    endResetModel();
}

QAbstractItemModel *QtTreeProxyModel::sourceModel() const
{
    Q_D(const QtTreeProxyModel);
    return d->model;
}

void QtTreeProxyModel::setGroupping(int column, int role)
{
    setGroupColumn(column);
    setGroupNameRole(role);
}

void QtTreeProxyModel::setGroupColumn(int column)
{
    Q_D(QtTreeProxyModel);
    if (d->groupColumn == column)
        return;

    d->groupColumn = column;
    onSourceReset();
    Q_EMIT groupColumnChanged(column);
}

int QtTreeProxyModel::groupColumn() const
{
    Q_D(const QtTreeProxyModel);
    return d->groupColumn;
}

void QtTreeProxyModel::setGroupNameRole(int role)
{
    Q_D(QtTreeProxyModel);
    if (d->groupRole == role)
        return;

    d->groupRole = role;
    onSourceReset();
    Q_EMIT groupNameRoleChanged(role);
}

int QtTreeProxyModel::groupNameRole() const
{
    Q_D(const QtTreeProxyModel);
    return d->groupRole;
}

QModelIndex QtTreeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    // Here is a tricky part:
    // - if we have a parent set internal id to it row
    // - otherwise set internal id to -1 sentinel
    // It's important that the -1 sentinel allows to
    // distinguish child/parent items
    if (parent.isValid())
        return createIndex(row, column, quintptr(parent.row()));
    else
        return createIndex(row, column, quintptr(-1));
}

QModelIndex QtTreeProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    // Here is a tricky part:
    // - if internal id equals -1 then the child is top level item: return the invalid model index
    // - if internal id is something other than -1 than the child is some child so we need to set
    // column of parent to 0 when creating the parent index, to escape selection issues

    if (child.internalId() == -1)
        return QModelIndex();
    else
        return createIndex(child.internalId(), 0, quintptr(-1));
}

int QtTreeProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const QtTreeProxyModel);
    if (parent.isValid())
        return (parent.internalId() == -1 ? d->groups[parent.row()].childCount : 0);
    else
        return d->groups.size();
}

int QtTreeProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_D(const QtTreeProxyModel);
    return d->model ? d->model->columnCount() : 0;
}

QVariant QtTreeProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    Q_D(const QtTreeProxyModel);
    if (!proxyIndex.isValid() || !d->model)
        return QVariant();

    const int id = proxyIndex.internalId();
    const int r = proxyIndex.row();
    const int c = proxyIndex.column();
    if (id == -1) // top-level item
    {
        if (c != d->groupColumn)
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        return d->groups[r].key;
    }

    const QModelIndex parent = proxyIndex.parent();
    if (!parent.isValid())
        return QVariant();

    const int row = d->sourceRowForGroup(d->groups[parent.row()].key, r);
    if (row == -1)
        return QVariant();
    else
        return d->model->index(row, c).data(role);
}

QVariant QtTreeProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const QtTreeProxyModel);
    if (d->model)
        return d->model->headerData(section, orientation, role);
    else
        return QAbstractItemModel::headerData(section, orientation, role);
}

void QtTreeProxyModel::onSourceRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_D(QtTreeProxyModel);
    if (!d->model)
        return;

    // populate groups from source model
    for (int i = first; i <= last; ++i)
    {
        QVariant value = d->model->index(i, d->groupColumn).data(d->groupRole);
        auto it = std::find(d->groups.begin(), d->groups.end(), value);
        if (it == d->groups.end())
            d->groups.emplace_back(value);
        else
            ++it->childCount;
    }
}

void QtTreeProxyModel::onSourceRowsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_D(QtTreeProxyModel);

    // populate groups from source model
    for (int i = first; i <= last; ++i)
    {
        QVariant value = d->model->index(i, d->groupColumn).data(d->groupRole);
        auto it = std::find(d->groups.begin(), d->groups.end(), value);
        if (it != d->groups.end())
            --it->childCount;

        if (it->childCount == 0)
            d->groups.erase(it);
    }
}

void QtTreeProxyModel::onSourceDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &roles)
{
    Q_D(QtTreeProxyModel);
    if (std::find(roles.begin(), roles.end(), d->groupRole))
        onSourceReset(); // update all groups
}

void QtTreeProxyModel::onSourceReset()
{
    Q_D(QtTreeProxyModel);
    d->groups.clear();

    if (!d->model)
        return;

    // populate groups from source model
    for (int i = 0; i < d->model->rowCount(); ++i)
    {
        QVariant value = sourceModel()->index(i, d->groupColumn).data(d->groupRole);
        auto it = std::find(d->groups.begin(), d->groups.end(), value);
        if (it == d->groups.end())
            d->groups.emplace_back(value);
        else
            ++it->childCount;
    }
}

void QtTreeProxyModel::onSourceDestroyed()
{
    Q_D(QtTreeProxyModel);
    beginResetModel();
    d->groups.clear();
    endResetModel();
}
