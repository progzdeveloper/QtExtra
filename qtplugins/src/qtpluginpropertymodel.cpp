#include "qtpluginpropertymodel.h"
#include "qtpluginmetadata.h"
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaClassInfo>
#include <QApplication>

typedef QHash<QString, QString> QStringHash;

class QtPluginPropertyModelPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtPluginPropertyModelPrivate)
public:
    enum MetaDataFields
    {
        IIDField = 0,
        KeyField,
        TitleField,
        PathField,
        LoadTypeField,
        FieldCount = LoadTypeField + 1
    };

    QtPluginMetadata metadata;
    const QMetaObject* metaObject;

    QtPluginPropertyModelPrivate() :
        metaObject(Q_NULLPTR) {}


    QString translateClassName() const
    {
        if (metaObject == Q_NULLPTR) {
            return tr("<Unknown>");
        }
        const char* className = metaObject->className();
        const int idx = metaObject->indexOfClassInfo(className);
        if (idx > 0)
            return tr(metaObject->classInfo(idx).value());
        else
            return className;
    }

    int classInfoCount() const
    {
        if (metaObject == Q_NULLPTR)
            return 0;
        const int count = (metaObject->classInfoCount() - metaObject->classInfoOffset());
        return (count - (metaObject->indexOfClassInfo(metaObject->className()) > 0));
    }

    int fieldCount() const
    {
        return (metaObject != Q_NULLPTR ? (classInfoCount() + FieldCount) : 0);
    }

    QVariant fieldName(int row) const
    {
        if (row < 0)
            return QVariant();
        switch(row) {
        case IIDField:   return tr("Interface");
        case KeyField:   return tr("Class Name");
        case TitleField: return tr("Title");
        case PathField:  return tr("Path");
        case LoadTypeField: return tr("Loading Type");
        default:
            break;
        }

        if (metaObject != Q_NULLPTR)
        {
            const int n = (metaObject->classInfoCount() - metaObject->classInfoOffset());
            const int i = (row - FieldCount);
            if (i < n)
                return metaObject->classInfo(metaObject->classInfoOffset() + i).name();
        }
        return QVariant();
    }

    QVariant fieldValue(int row) const
    {
        if (row < 0)
            return QVariant();

        switch(row)
        {
        case IIDField: return metadata.iid;
        case KeyField: return metadata.key;
        case TitleField: return translateClassName();
        case PathField: return metadata.libPath;
        case LoadTypeField: return (metadata.isStatic ? tr("Static") : tr("Dynamic"));
        default:
            break;
        }

        if (metaObject != Q_NULLPTR) {
            const int n = (metaObject->classInfoCount() - metaObject->classInfoOffset());
            int i = (row - FieldCount);
            i += (qstrcmp(metaObject->classInfo(i).name(), metaObject->className()) == 0);
            if (i < n) {
                return metaObject->classInfo((metaObject->classInfoOffset() + i)).value();
            }
        }

        return QVariant();
    }
};



QtPluginPropertyModel::QtPluginPropertyModel( QObject *parent /*= 0*/ ) :
    QAbstractTableModel(parent), d_ptr(new QtPluginPropertyModelPrivate)
{
}

QtPluginPropertyModel::~QtPluginPropertyModel()
{
}

void QtPluginPropertyModel::setMetadata( const QtPluginMetadata& metadata )
{
    Q_D(QtPluginPropertyModel);
    beginResetModel();
    d->metadata = metadata;
    d->metaObject = Q_NULLPTR;
    if (d->metadata.instance != Q_NULLPTR)
        d->metaObject = metadata.instance->metaObject();
    endResetModel();
}

int QtPluginPropertyModel::rowCount( const QModelIndex& parent) const
{
    Q_D(const QtPluginPropertyModel);
    if (parent.isValid())
        return 0;
    return d->fieldCount();
}

QVariant QtPluginPropertyModel::data( const QModelIndex &index, int role) const
{
    Q_D(const QtPluginPropertyModel);
    if (!index.isValid()) {
        return QVariant();
    }

    const int row = index.row();
    if (row < 0 || row >= d->fieldCount()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
        switch(index.column()) {
        case TitleColumn:
            return d->fieldName(index.row());
        case ValueColumn:
            return d->fieldValue(index.row());
        }
    }
    return QVariant();
}


QVariant QtPluginPropertyModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section) {
        case TitleColumn: return tr("Property");
        case ValueColumn: return tr("Value");
        }
    }
    return QVariant();
}

void QtPluginPropertyModel::clear()
{
    Q_D(QtPluginPropertyModel);
    beginResetModel();
    d->metaObject = Q_NULLPTR;
    d->metadata.instance = Q_NULLPTR;
    d->metadata.key.clear();
    d->metadata.iid.clear();
    d->metadata.libPath.clear();
    d->metadata.errorString.clear();
    d->metadata.isLoaded = false;
    d->metadata.isStatic = false;
    endResetModel();
}
