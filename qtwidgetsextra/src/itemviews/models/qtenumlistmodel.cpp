#include "qtenumlistmodel.h"
#include <QMetaEnum>
#include <QBitArray>
#include <QIcon>

struct QEnumElement
{
    int value;
    QString text;
};


class QtEnumListModelPrivate
{
public:
    QtEnumListModel* q_ptr;

    bool checkable;
    QMetaEnum metaEnum;
    QVector<QEnumElement> elements;
    QBitArray bitmask;
    QtEnumListModelPrivate(QtEnumListModel* q) : q_ptr(q) {}
};

QtEnumListModel::QtEnumListModel(QObject* parent) : 
    QAbstractListModel(parent), d_ptr(new QtEnumListModelPrivate(this))
{
}

QtEnumListModel::QtEnumListModel(const QMetaEnum& e, QObject* parent) :
    QAbstractListModel(parent), d_ptr(new QtEnumListModelPrivate(this))
{
    setEnum(e);
}

QtEnumListModel::~QtEnumListModel()
{
    delete d_ptr;
}

void QtEnumListModel::setMeta(const QMetaEnum & e)
{
    Q_D(QtEnumListModel);
    d->metaEnum = e;
}

void QtEnumListModel::setEnum(const QMetaEnum & e)
{
    Q_D(QtEnumListModel);

    clear();
    d->metaEnum = e;
    if (!e.isValid())
        return;

    int n = e.keyCount();
    d->checkable = e.isFlag();
    d->elements.resize(n);
    d->bitmask.resize(n);
    d->bitmask.fill(false);

    beginInsertRows(QModelIndex(), 0, n - 1);
    for (int i = 0; i < n; i++) {
        d->elements[i] = { e.value(i), e.key(i) };
    }
    endInsertRows();
}

QMetaEnum QtEnumListModel::metaEnum() const
{
    Q_D(const QtEnumListModel);
    return d->metaEnum;
}

void QtEnumListModel::setCheckable(bool on)
{
    Q_D(QtEnumListModel);
    d->checkable = on;
}

bool QtEnumListModel::isCheckable() const
{
    Q_D(const QtEnumListModel);
    return d->checkable;
}

void QtEnumListModel::clear()
{
    Q_D(QtEnumListModel);
    d->checkable = false;
    if (!d->elements.empty()) {
        beginResetModel();
        d->elements.clear();
        endResetModel();
    }
}

void QtEnumListModel::revert()
{
    Q_D(QtEnumListModel);
    beginResetModel();
    d->bitmask.fill(false);
    endResetModel();
}


int QtEnumListModel::rowCount(const QModelIndex & parent) const
{
    Q_D(const QtEnumListModel);
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return d->elements.size();
}

Qt::ItemFlags QtEnumListModel::flags(const QModelIndex & index) const
{
    Q_D(const QtEnumListModel);
    Qt::ItemFlags f = QAbstractListModel::flags(index);
    return (d->checkable ? (f | Qt::ItemIsUserCheckable) : f);
}

bool QtEnumListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    Q_D(QtEnumListModel);
    if (!index.isValid() || role != Qt::CheckStateRole || index.column() > 0)
        return false;

    Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());
    bool state = (checkState == Qt::Checked);
    if (d->bitmask.testBit(index.row()) != state) {
        d->bitmask.setBit(index.row(), state);
        Q_EMIT dataChanged(index, index, QVector<int>() << Qt::CheckStateRole);
        return true;
    }
    return false;
}

QVariant QtEnumListModel::data(const QModelIndex & index, int role) const
{
    Q_D(const QtEnumListModel);
    if (!index.isValid() || index.column() > 0)
        return QVariant();


    int row = index.row();
    switch (role)
    {
    case Qt::DisplayRole:
        return d->elements[row].text;
    case Qt::EditRole:
        return d->elements[row].value;
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
        return d->elements[row].text;
    case Qt::CheckStateRole:
        if (d->checkable)
            return (d->bitmask[row] ? Qt::Checked : Qt::Unchecked);
    }
    return QVariant();
}




