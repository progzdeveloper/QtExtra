#include <algorithm>
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QCoreApplication>
#include "qtobjectlistmodel.h"

namespace
{

inline QString className(const QMetaObject* metaObject)
{
    const char* name = metaObject->className();
    for (int i = metaObject->classInfoOffset(), n = metaObject->classInfoCount(); i < n; i++) {
        if (qstrcmp(metaObject->classInfo(i).name(), name) == 0) {
            return QCoreApplication::translate(name, metaObject->classInfo(i).value());
        }
    }
    return QCoreApplication::translate(name, name);
}

}

QtObjectListModel::QtObjectListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QtObjectListModel::~QtObjectListModel()
{
}

void QtObjectListModel::insert(QObject *object)
{
    if (object != Q_NULLPTR) {
        beginInsertRows(QModelIndex(), mObjects.size(), mObjects.size());
        mObjects.push_back(object);
        connect(object, &QObject::destroyed, this, &QtObjectListModel::objectDestroyed);
        endInsertRows();
    }
}

void QtObjectListModel::insert(const QObjectList &list)
{
    for (auto it = mObjects.cbegin(); it != mObjects.cend(); ++it) {
        disconnect(*it, &QObject::destroyed, this, &QtObjectListModel::objectDestroyed);
    }

    if (list.isEmpty())
        return;

    beginInsertRows(QModelIndex(), mObjects.size(), mObjects.size() + list.size());
    std::copy(list.cbegin(), list.cend(), std::back_inserter(mObjects));
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        connect(*it, &QObject::destroyed, this, &QtObjectListModel::objectDestroyed);
    }
    endInsertRows();
}

void QtObjectListModel::remove(QObject *object)
{
    int i = mObjects.indexOf(object);
    if (i == -1)
        return;
    beginRemoveRows(QModelIndex(), i, i);
    mObjects.removeAt(i);
    endRemoveRows();
}

bool QtObjectListModel::remove(int row, int count)
{
    if (row < 0)
        return false;
    if ((row + count) >= mObjects.size())
        return false;

    beginRemoveRows(QModelIndex(), row, count-1);
    mObjects.erase(mObjects.begin() + row, mObjects.begin() + (row + count));
    endRemoveRows();
    return true;
}

bool QtObjectListModel::setObject(int row, QObject *object)
{
    static const QVector<int> roles = { Qt::DisplayRole,
                                        Qt::EditRole,
                                        Qt::ToolTipRole };
    if (object == Q_NULLPTR)
        return false;

    if (row < 0 || row >= mObjects.size())
        return false;

    mObjects[row] = object;
    QModelIndex idx = index(row);
    Q_EMIT dataChanged(idx, idx, roles);
    return true;
}


QObject *QtObjectListModel::objectAt(int row) const
{
    if (row < 0 || row >= mObjects.size())
        return Q_NULLPTR;
    return mObjects.at(row);
}

QObjectList QtObjectListModel::objects() const
{
    return mObjects;
}


void QtObjectListModel::reset()
{
    beginRemoveRows(QModelIndex(), 0, mObjects.count()-1);
    mObjects.clear();
    endRemoveRows();
}

void QtObjectListModel::objectDestroyed(QObject *object)
{
    int index = mObjects.indexOf(object);
    if (index != -1) {
        beginRemoveRows(QModelIndex(), index, index);
        mObjects.removeAt(index);
        endRemoveRows();
    }
}

int QtObjectListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return mObjects.size();
}

QVariant QtObjectListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.column() != 0)
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= mObjects.size())
        return QVariant();

    QObject* object = mObjects[row];
    switch(role) {
        /*case Qt::ToolTipRole:
        case Qt::StatusTipRole:
            return tr("%1 [%2]").arg(object->objectName(), className(object->metaObject()));
        */
        case Qt::DisplayRole:
            return className(object->metaObject());
        case Qt::EditRole:
            return object->objectName();
    }

    return QVariant();
}

bool QtObjectListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (index.column() != 0)
        return false;

    int row = index.row();
    if (row < 0 || row >= mObjects.size())
        return false;

    if (data(index, role) != value && role == Qt::EditRole) {
        mObjects[row]->setObjectName(value.toString());
        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags QtObjectListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
}


/*bool QtObjectListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool QtObjectListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}*/
