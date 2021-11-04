#include "qtvariantlistmodel.h"

class QtVariantListModelPrivate
{
public:
    QVariantList list;
    QString header;
    bool editable;

    QtVariantListModelPrivate() :
        editable(true) {
    }

    QtVariantListModelPrivate(const QVariantList& values) :
        list(values),
        editable(false){
    }

    inline void write(int row, const QVariant& v) {
        *(list.begin() + row) = v;
    }

    inline const QVariant& read(int row) const {
        return list.at(row);
    }
};


QtVariantListModel::QtVariantListModel(QObject *parent) :
    QAbstractListModel(parent),
    d_ptr(new QtVariantListModelPrivate)
{
}

QtVariantListModel::QtVariantListModel(const QVariantList &list, QObject *parent) :
    QAbstractListModel(parent),
    d_ptr(new QtVariantListModelPrivate(list))
{
}

QtVariantListModel::~QtVariantListModel()
{
}

void QtVariantListModel::setEditable(bool on)
{
    Q_D(QtVariantListModel);
    if (d->editable != on) {
        d->editable = on;
        Q_EMIT editableChanged(on);
    }
}

bool QtVariantListModel::isEditable() const
{
    Q_D(const QtVariantListModel);
    return d->editable;
}

void QtVariantListModel::setValues(const QVariantList &list)
{
    Q_D(QtVariantListModel);
    beginResetModel();
    d->list = list;
    endResetModel();
}

QVariantList QtVariantListModel::values() const
{
    Q_D(const QtVariantListModel);
    return d->list;
}

void QtVariantListModel::clear()
{
    Q_D(QtVariantListModel);
    beginResetModel();
    d->list.clear();
    endResetModel();
}

QVariant QtVariantListModel::defaultValue() const
{
    return QString("Untitled%1").arg(rowCount());
}

QVariant QtVariantListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const QtVariantListModel);
    if (section == 0 && orientation == Qt::Horizontal &&
        (role == Qt::DisplayRole || role == Qt::EditRole))
        return d->header;
    return QVariant();
}

bool QtVariantListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    Q_D(QtVariantListModel);
    if (section == 0 && orientation == Qt::Horizontal && role == Qt::EditRole) {
        QString text = value.toString();
        if (text != d->header) {
            d->header = text;
            emit headerDataChanged(orientation, section, section);
            return true;
        }
    }
    return false;
}

int QtVariantListModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const QtVariantListModel);
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return d->list.size();
}

QVariant QtVariantListModel::data(const QModelIndex &index, int role) const
{
    Q_D(const QtVariantListModel);
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int column = index.column();
    if (column != 0 || (row < 0 || row >= d->list.size()))
        return QVariant();

    const QVariant& v = d->read(row);
    switch (role) {
    case Qt::EditRole:
        return v;
    case Qt::DisplayRole:
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
        return v.toString();
    default:
        break;
    }
    return QVariant();
}

bool QtVariantListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(QtVariantListModel);
    if(!d->editable || !index.isValid() || role != Qt::EditRole) {
        return false;
    }

    int row = index.row();
    int column = index.column();
    if (column != 0 || (row < 0 || row >= d->list.size())) {
        return false;
    }

    if (d->read(row) != value) {
        d->write(row, value);
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }

    return false;
}

Qt::ItemFlags QtVariantListModel::flags(const QModelIndex &index) const
{
    Q_D(const QtVariantListModel);
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (d->editable)
        return (Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable);
    else
        return (Qt::ItemIsEnabled|Qt::ItemIsSelectable);
}

bool QtVariantListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_D(QtVariantListModel);

    if(!d->editable) {
        return false;
    }
    if (row < 0 || row > d->list.size()) {
        return false;
    }
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; i++) {
        d->list.insert(d->list.begin() + row, defaultValue());
    }
    endInsertRows();
    return true;

}

bool QtVariantListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_D(QtVariantListModel);

    if(!d->editable) {
        return false;
    }
    if (row < 0 || (row + count) > d->list.size()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    d->list.erase(d->list.begin() + row, d->list.begin() + (row + count));
    endRemoveRows();
    return true;
}

bool QtVariantListModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                                  const QModelIndex &destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(sourceRow)
    Q_UNUSED(count)
    Q_UNUSED(destinationParent)
    Q_UNUSED(destinationChild)
    //Q_D(QtVariantListModel);
    return false;
}





