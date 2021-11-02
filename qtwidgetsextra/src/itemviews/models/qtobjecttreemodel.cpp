#include "qtobjecttreemodel.h"
#include <QChildEvent>

QtObjectTreeModel::QtObjectTreeModel(QObject *root, QObject *parent) :
    QAbstractItemModel(parent), mRoot(Q_NULLPTR), mEditable(false)
{
    setRootObject(root);
}

void QtObjectTreeModel::setRootObject(QObject *root)
{
    if (mRoot != root) {
        if (mRoot != Q_NULLPTR)
            mRoot->removeEventFilter(this);

        mRoot = root;
        if (mRoot != Q_NULLPTR) {
            mRoot->installEventFilter(this);
        }
        beginResetModel();
        endResetModel();
        Q_EMIT rootObjectChanged(mRoot);
    }
}

QObject *QtObjectTreeModel::rootObject() const
{
    return mRoot;
}

QObject *QtObjectTreeModel::object(const QModelIndex &index) const
{
    if (!index.isValid())
        return Q_NULLPTR;

    if (index.model() != this)
        return Q_NULLPTR;

    return static_cast<QObject*>(index.internalPointer());
}


void QtObjectTreeModel::setEditable(bool on)
{
    if (mEditable != on) {
        mEditable = on;
        Q_EMIT editableChanged(mEditable);
    }
}

bool QtObjectTreeModel::isEditable() const
{
    return mEditable;
}

QVariant QtObjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole || orientation != Qt::Horizontal )
        return QVariant();

    switch( section )
    {
    case 0:  return tr( "Object" );
    case 1:  return tr( "Class" );
    default: return QVariant();
    }
}

QModelIndex QtObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    QObject *parentObject;

     if( !parent.isValid() )
       parentObject = mRoot;
     else
       parentObject = static_cast<QObject*>( parent.internalPointer() );

     if( row < parentObject->children().count() )
       return createIndex( row, column, parentObject->children().at( row ) );
     else
       return QModelIndex();
}

QModelIndex QtObjectTreeModel::parent(const QModelIndex &index) const
{
    if( !index.isValid() )
       return QModelIndex();

     QObject *indexObject = static_cast<QObject*>( index.internalPointer() );
     QObject *parentObject = indexObject->parent();

     if( parentObject == mRoot )
       return QModelIndex();

     QObject *grandParentObject = parentObject->parent();

     return createIndex( grandParentObject->children().indexOf( parentObject ), 0, parentObject );
}

int QtObjectTreeModel::rowCount(const QModelIndex &parent) const
{
    QObject *parentObject;

     if( !parent.isValid() )
       parentObject = mRoot;
     else
       parentObject = static_cast<QObject*>( parent.internalPointer() );

     return parentObject->children().count();
}

int QtObjectTreeModel::columnCount(const QModelIndex &parent) const
{
    /*if (!parent.isValid())
        return 0;*/
    Q_UNUSED(parent)
    return 2;
}

QVariant QtObjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if( role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole )
    {
        QObject* object = static_cast<QObject*>( index.internalPointer() );
        switch( index.column() )
        {
        case 0:
            return (object != Q_NULLPTR ? object->objectName() : QString());
        case 1:
            return (object != Q_NULLPTR ? object->metaObject()->className() : QString());
        default:
            break;
        }
    }

    return QVariant();
}

bool QtObjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!mEditable)
        return false;

    if (!index.isValid())
        return false;

    if (index.column() != 0 || role != Qt::EditRole)
        return false;

    if (data(index, role) != value) {
        QObject* object = static_cast<QObject*>( index.internalPointer() );
        if (object != Q_NULLPTR) {
            object->setObjectName(value.toString());
            emit dataChanged(index, index, QVector<int>() << role);
            return true;
        }
    }
    return false;
}

Qt::ItemFlags QtObjectTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return (mEditable ? flags | Qt::ItemIsEditable : flags);
}

bool QtObjectTreeModel::eventFilter(QObject *watched, QEvent *event)
{
    const int eventType = event->type();
    if (watched == mRoot && (eventType == QEvent::ChildAdded || eventType == QEvent::ChildRemoved))
    {
        //QChildEvent* childEvent = static_cast<QChildEvent*>(event);
        /*if (childEvent->added()) {
            int n = rowCount();
            beginInsertRows(QModelIndex(), n, n);
            endInsertRows();
        }

        if (childEvent->removed()) {
            int row = mRoot->children().indexOf(childEvent->child());
            beginRemoveRows(QModelIndex(), row, row);
            endRemoveRows();
        }*/
        //Q_EMIT layoutChanged();
    }

    return QAbstractItemModel::eventFilter(watched, event);
}


