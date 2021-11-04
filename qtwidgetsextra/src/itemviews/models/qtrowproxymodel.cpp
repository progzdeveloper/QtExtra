#include "qtrowproxymodel.h"
#include <QCoreApplication>

class QtRowProxyModelPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtRowProxyModelPrivate)
public:
    QModelIndex rootIndex;
    QString headers[2];
    int currentRow;
    bool readOnly;
    bool titleEditable;

    QtRowProxyModelPrivate() :
        currentRow(-1)
    {
        headers[0] = tr("Field");
        headers[1] = tr("Value");
    }

    QModelIndex mapIndex(const QAbstractItemModel *sourceModel, int row) const;
};

QModelIndex QtRowProxyModelPrivate::mapIndex(const QAbstractItemModel* sourceModel, int row) const
{
    if (!sourceModel)
        return QModelIndex();

    return sourceModel->index(currentRow, row, rootIndex);
}


QtRowProxyModel::QtRowProxyModel(QObject *parent) :
    QIdentityProxyModel(parent),
    d_ptr(new QtRowProxyModelPrivate)
{
}

QtRowProxyModel::~QtRowProxyModel()
{
}

void QtRowProxyModel::setRootIndex(const QModelIndex &rootIndex)
{
    Q_D(QtRowProxyModel);
    if (d->rootIndex != rootIndex) {
        beginResetModel();
        d->rootIndex = rootIndex;
        endResetModel();
        Q_EMIT rootIndexChanged(rootIndex);
    }
}

QModelIndex QtRowProxyModel::rootIndex() const
{
    Q_D(const QtRowProxyModel);
    return d->rootIndex;
}

void QtRowProxyModel::setCurrentRow(int row)
{
    Q_D(QtRowProxyModel);
    if (d->currentRow != row) {
        beginResetModel();
        d->currentRow = row;
        endResetModel();
    }
}

int QtRowProxyModel::currentRow() const
{
    Q_D(const QtRowProxyModel);
    return d->currentRow;
}

void QtRowProxyModel::setReadOnly(bool on)
{
    Q_D(QtRowProxyModel);
    if (d->readOnly != on) {
        beginResetModel();
        d->readOnly = on;
        endResetModel();
    }
}

bool QtRowProxyModel::isReadOnly() const
{
    Q_D(const QtRowProxyModel);
    return d->readOnly;
}

void QtRowProxyModel::setTitleEditable(bool on)
{
    Q_D(QtRowProxyModel);
    if (!d->titleEditable) {
        if (!d->readOnly) {
            beginResetModel();
            d->titleEditable = on;
            endResetModel();
        } else {
            d->titleEditable = on;
        }
    }
}

bool QtRowProxyModel::isTitleEditable() const
{
    Q_D(const QtRowProxyModel);
    return (d->readOnly ? false : d->titleEditable);
}

QVariant QtRowProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const QtRowProxyModel);
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section < 0 || section > 1)
            return QVariant();
        return d->headers[section];
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

bool QtRowProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{

    if (orientation != Qt::Horizontal)
        return false;
    if (section < 0 || section > 1)
        return false;

    if (value != headerData(section, orientation, role)) {
        Q_D(QtRowProxyModel);
        d->headers[section] = value.toString();
        Q_EMIT headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int QtRowProxyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (!sourceModel())
        return 0;

    Q_D(const QtRowProxyModel);
    return sourceModel()->columnCount(d->rootIndex);
}

int QtRowProxyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2; // allways 2 columns
}

QVariant QtRowProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!sourceModel())
        return QVariant();

    int column = index.column();
    int row = index.row();

    if (column < 0 || column > 1)
        return QVariant();

    Q_D(const QtRowProxyModel);
    if (row < 0 || row >= sourceModel()->columnCount(d->rootIndex))
        return QVariant();

    switch (column) {
    case 0:
        return sourceModel()->headerData(index.row(), Qt::Horizontal, role);
    case 1:
        return sourceModel()->data(d->mapIndex(sourceModel(), index.row()), role);
    default:
        break;
    }
    return QVariant();
}

bool QtRowProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (!sourceModel())
        return false;
    Q_D(const QtRowProxyModel);
    bool success = false;
    if (data(index, role) != value)
    {
        switch (index.column())
        {
        case 0:
            success = (sourceModel()->setHeaderData(index.row(), Qt::Horizontal, value, role));
            break;
        case 1:
            success = (sourceModel()->setData(d->mapIndex(sourceModel(), index.row()), value, role));
            break;
        default:
            break;
        }
    }

    if (success) {
        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags QtRowProxyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (!sourceModel())
        return Qt::NoItemFlags;

    Q_D(const QtRowProxyModel);
    if (d->readOnly) {
        return (Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    }
    if (index.column() == 1) {
        return sourceModel()->flags(mapToSource(index));
    }
    return (d->titleEditable ? Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled : Qt::ItemIsEnabled);
}

/*QModelIndex QtRowProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!sourceModel())
        return QModelIndex();
    if (proxyIndex.column() == 0)
        return QModelIndex();

    Q_D(const QtRowProxyModel);
    return sourceModel()->index(d->currentRow, proxyIndex.row(), d->rootIndex);
}*/

/*QModelIndex QtRowProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceModel())
        return QModelIndex();

    Q_D(const QtRowProxyModel);
    if (sourceIndex.parent() != d->rootIndex)
        return QModelIndex();
    // ### that is questionable ...
    return createIndex(1, sourceIndex.column(), d->rootIndex);
}*/

