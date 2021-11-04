#include <vector>
#include "qtcolumnproxymodel.h"
#include <QApplication>

class QtColumnProxyModelPrivate
{
public:
    QVector<int> m_sourceColumns;

    int proxyColumnForSourceColumn(int sourceColumn) const;
    int sourceColumnForProxyColumn(int proxyColumn) const;
};


int QtColumnProxyModelPrivate::proxyColumnForSourceColumn(int sourceColumn) const
{
    // If this is too slow, we could add a second QVector with index=logical_source_column value=desired_pos_in_proxy.
    auto it = std::find(m_sourceColumns.cbegin(), m_sourceColumns.cend(), sourceColumn);
    return (it != m_sourceColumns.end() ? *it : -1);
}

int QtColumnProxyModelPrivate::sourceColumnForProxyColumn(int proxyColumn) const
{
    Q_ASSERT(proxyColumn >= 0);
    Q_ASSERT(proxyColumn < m_sourceColumns.size());
    return m_sourceColumns[proxyColumn];
}



QtColumnProxyModel::QtColumnProxyModel(QObject *parent) :
    QIdentityProxyModel(parent),
    d_ptr(new QtColumnProxyModelPrivate)
{
}

QtColumnProxyModel::~QtColumnProxyModel()
{
}

void QtColumnProxyModel::setSourceColumns(const QVector<int> &columns)
{
    Q_D(QtColumnProxyModel);
    Q_EMIT layoutAboutToBeChanged();
    //beginResetModel();
    d->m_sourceColumns = columns;
    //endResetModel();
    Q_EMIT layoutChanged();

}

const QVector<int> &QtColumnProxyModel::sourceColumns() const
{
    Q_D(const QtColumnProxyModel);
    return d->m_sourceColumns;
}

int QtColumnProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    Q_D(const QtColumnProxyModel);
    if (!sourceModel()) {
        return 0;
    }
    return d->m_sourceColumns.size();
}

int QtColumnProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    if (!sourceModel()) {
        return 0;
    }
    // The parent in the source model is on column 0, whatever swapping we are doing
    const QModelIndex sourceParent = mapToSource(parent).sibling(parent.row(), 0);
    return sourceModel()->rowCount(sourceParent);
}

// We derive from QIdentityProxyModel simply to be able to use
// its mapFromSource method which has friend access to createIndex() in the source model.

QModelIndex QtColumnProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);

    if (row < 0 || column < 0)
        return QModelIndex();

    Q_D(const QtColumnProxyModel);

    // The parent in the source model is on column 0, whatever swapping we are doing
    const QModelIndex sourceParent = mapToSource(parent).sibling(parent.row(), 0);

    // Find the child in the source model, we need its internal pointer
    const QModelIndex sourceIndex = sourceModel()->index(row, d->sourceColumnForProxyColumn(column), sourceParent);
    if (!sourceIndex.isValid()) {
        return QModelIndex();
    }

    return createIndex(row, column, sourceIndex.internalPointer());
}

QModelIndex QtColumnProxyModel::parent(const QModelIndex &child) const
{
    Q_ASSERT(child.isValid() ? child.model() == this : true);

    const QModelIndex sourceIndex = mapToSource(child);
    const QModelIndex sourceParent = sourceIndex.parent();
    if (!sourceParent.isValid()) {
        return QModelIndex();
    }
    return createIndex(sourceParent.row(), 0, sourceParent.internalPointer());
}

QVariant QtColumnProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const QtColumnProxyModel);
    if (orientation == Qt::Horizontal) {
        const int sourceCol = d->sourceColumnForProxyColumn(section);
        return sourceModel()->headerData(sourceCol, orientation, role);
    } else {
        return QIdentityProxyModel::headerData(section, orientation, role);
    }
}

QModelIndex QtColumnProxyModel::sibling(int row, int column, const QModelIndex &idx) const
{
    Q_D(const QtColumnProxyModel);
    if (column >= d->m_sourceColumns.size()) {
        return QModelIndex();
    }
    return index(row, column, idx.parent());
}

QModelIndex QtColumnProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    Q_D(const QtColumnProxyModel);
    if (!sourceIndex.isValid()) {
        return QModelIndex();
    }
    Q_ASSERT(sourceIndex.model() == sourceModel());
    const int proxyColumn = d->proxyColumnForSourceColumn(sourceIndex.column());
    return createIndex(sourceIndex.row(), proxyColumn, sourceIndex.internalPointer());
}

QModelIndex QtColumnProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    Q_D(const QtColumnProxyModel);
    if (!proxyIndex.isValid()) {
        return QModelIndex();
    }
    // This is just an indirect way to call sourceModel->createIndex(row, sourceColumn, pointer)
    const QModelIndex fakeIndex = createIndex(proxyIndex.row(), d->sourceColumnForProxyColumn(proxyIndex.column()), proxyIndex.internalPointer());
    return QIdentityProxyModel::mapToSource(fakeIndex);
}


