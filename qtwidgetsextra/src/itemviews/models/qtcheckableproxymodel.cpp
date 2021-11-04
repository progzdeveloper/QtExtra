#include <vector>
#include <algorithm>
#include <functional>

#include <QDebug>

#include "qtcheckableproxymodel.h"

class QtCheckableProxyModelPrivate
{
public:
    std::vector<bool> bits;
    int column;
    QtCheckableProxyModelPrivate();
};

QtCheckableProxyModelPrivate::QtCheckableProxyModelPrivate() :
    column(0) {}


QtCheckableProxyModel::QtCheckableProxyModel(QObject *parent) :
    BaseModel(parent),
    d_ptr(new QtCheckableProxyModelPrivate)
{
    setFilterRole(Qt::CheckStateRole);
    connect(this, SIGNAL(sourceModelChanged()), SLOT(resetInternals()));
}

QtCheckableProxyModel::~QtCheckableProxyModel()
{
}

void QtCheckableProxyModel::setSourceModel(QAbstractItemModel *model)
{
    Q_D(QtCheckableProxyModel);
    if (sourceModel())
    {
        disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &QtCheckableProxyModel::insert);
        disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &QtCheckableProxyModel::remove);
    }

    BaseModel::setSourceModel(model);

    if (model != Q_NULLPTR)
    {
        d->bits.resize(model->rowCount());
        std::fill(d->bits.begin(), d->bits.end(), true);
        connect(model, &QAbstractItemModel::rowsInserted, this, &QtCheckableProxyModel::insert);
        connect(model, &QAbstractItemModel::rowsRemoved, this, &QtCheckableProxyModel::remove);
    }
}

void QtCheckableProxyModel::setModelColumn(int column)
{
    Q_D(QtCheckableProxyModel);
    d->column = column;
}

int QtCheckableProxyModel::modelColumn() const
{
    Q_D(const QtCheckableProxyModel);
    return d->column;
}

void QtCheckableProxyModel::setCheckState(Qt::CheckState state)
{
    Q_D(QtCheckableProxyModel);

    if (checkState() == state)
        return;

    std::fill(d->bits.begin(), d->bits.end(), (state == Qt::Checked));
    QModelIndex first = index(0, d->column);
    QModelIndex last = index(rowCount()-1, d->column);
    Q_EMIT dataChanged(first, last, QVector<int>() << Qt::CheckStateRole);
}

Qt::CheckState QtCheckableProxyModel::checkState() const
{
    Q_D(const QtCheckableProxyModel);

    int n = std::count(d->bits.begin(), d->bits.end(), true);
    if (n == 0)
        return Qt::Unchecked;
    else
        return (size_t(n) == d->bits.size() ? Qt::Checked : Qt::PartiallyChecked);
}

bool QtCheckableProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    Q_D(QtCheckableProxyModel);
    if (role == Qt::CheckStateRole && d->column == index.column())
    {
        int row = mapToSource(index).row();
        bool state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked);
        if (d->bits[row] != state) {
            d->bits[row] = state;
            Q_EMIT dataChanged(index, index, QVector<int>() << role);
            return true;
        }
    }
    return BaseModel::setData(index, value, role);
}

QVariant QtCheckableProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Q_D(const QtCheckableProxyModel);

    if (role == Qt::CheckStateRole && d->column == index.column())
    {
        int row = mapToSource(index).row();
        if (row < 0)
            return QVariant();
        if (size_t(row) >= d->bits.size())
            return QVariant();
        return (int)(d->bits[row] ? Qt::Checked : Qt::Unchecked);
    }
    return BaseModel::data(index, role);
}

Qt::ItemFlags QtCheckableProxyModel::flags(const QModelIndex &index) const
{
    Q_D(const QtCheckableProxyModel);
    if (index.isValid() && index.column() == d->column)
        return BaseModel::flags(index) | Qt::ItemIsUserCheckable;
    return  BaseModel::flags(index);
}


void QtCheckableProxyModel::setChecked()
{
    setCheckState(Qt::Checked);
}

void QtCheckableProxyModel::setUnchecked()
{
    setCheckState(Qt::Unchecked);
}

void QtCheckableProxyModel::invert()
{
    Q_D(QtCheckableProxyModel);

    std::transform(d->bits.begin(), d->bits.end(), d->bits.begin(), std::logical_not<bool>());
    QModelIndex first = index(0, d->column);
    QModelIndex last  = index(rowCount()-1, d->column);
    Q_EMIT dataChanged(first, last, QVector<int>() << Qt::CheckStateRole);
}

void QtCheckableProxyModel::filterChecked()
{
    setFilterFixedString(QString(Qt::Checked));
}

void QtCheckableProxyModel::clearFilter()
{
    setFilterFixedString(QString());
}


void QtCheckableProxyModel::insert(const QModelIndex &, int row, int count)
{
    Q_D(QtCheckableProxyModel);
    d->bits.insert(d->bits.begin() + row, count + 1, true);
}

void QtCheckableProxyModel::remove(const QModelIndex &, int row, int count)
{
    Q_D(QtCheckableProxyModel);
    d->bits.erase(d->bits.begin() + row, d->bits.begin() + (row + count));
}

void QtCheckableProxyModel::resetInternals()
{
    Q_D(QtCheckableProxyModel);
    d->bits.resize(rowCount());
}


bool QtCheckableProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return BaseModel::insertRows(row, count, parent);
}

bool QtCheckableProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return BaseModel::removeRows(row, count, parent);
}



