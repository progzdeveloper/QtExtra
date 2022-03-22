#include "qtradiusesmodel.h"

QtRadiusesModel::QtRadiusesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant QtRadiusesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();

    switch (section)
    {
    case 0: return tr("Corner");
    case 1: return tr("Radius");
    }
    return QVariant();
}

int QtRadiusesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return radiusesArray.size();
}

int QtRadiusesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant QtRadiusesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int row = index.row();
    const int col = index.column();
    if ((col < 0 || col > 1) || (row < 0 || row >= radiusesArray.size()))
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (col)
        {
        case 0: return tr("Corner #%1").arg(row);
        case 1: return radiusesArray[row];
        }
    }
    return QVariant();
}

bool QtRadiusesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    const int row = index.row();
    const int col = index.column();

    if ((col != 1) || (row < 0 || row >= radiusesArray.size()) || role != Qt::EditRole)
        return false;

    const double radius = value.toDouble();
    if (radiusesArray[row] == radius)
        return false;

    radiusesArray[row] = radius;
    Q_EMIT dataChanged(index, index, QVector<int>() << role);
    return true;
}

Qt::ItemFlags QtRadiusesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    if (index.column() == 1)
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled;
}

void QtRadiusesModel::reset(int count)
{
    beginResetModel();
    radiusesArray.resize(count);
    std::fill(radiusesArray.begin(), radiusesArray.end(), 0.0);
    endResetModel();
    Q_EMIT dataChanged(index(0, 1), index(radiusesArray.size(), 1));
}

const RadiusesVector& QtRadiusesModel::radiuses() const
{
    return radiusesArray;
}
