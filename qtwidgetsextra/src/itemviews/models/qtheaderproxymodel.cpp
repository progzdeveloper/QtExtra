#include "qtheaderproxymodel.h"

QtHeaderProxyModel::QtHeaderProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

QtHeaderProxyModel::~QtHeaderProxyModel()
{
}

QVariant QtHeaderProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return tr("Headers");

    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return section;

    return QVariant();
}

int QtHeaderProxyModel::rowCount(const QModelIndex &/*parent*/) const
{
    if (sourceModel())
        return sourceModel()->columnCount();
    return 0;
}

int QtHeaderProxyModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QVariant QtHeaderProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (sourceModel())
        return sourceModel()->headerData(mapToSource(index).row(), Qt::Horizontal, role);
    return QVariant();
}
