#include "qtsqlitemtreemodel.h"
#include <QtSql>


class QtSqlItem
{
public:
    QtSqlItem(int type, const QString& title, QtSqlItem* parent = Q_NULLPTR);
    ~QtSqlItem();

    int type() const;

    QtSqlItem *parent() const;
    QtSqlItem *childAt(int number);

    int childCount() const;
    int childNumber() const;

    QString title() const;
    void setTitle(const QString &value);

    void appendChild(QtSqlItem* item);
    bool removeChildren(int position, int count);
    bool removeChild(int position);

private:
    QList<QtSqlItem*> mChildren;
    QtSqlItem *mParent;
    QString mTitle;
    int mType;
};



QtSqlItem::QtSqlItem(int type, const QString &title, QtSqlItem *parent) :
    mParent(parent), mTitle(title), mType(type)
{
}

QtSqlItem::~QtSqlItem()
{
    qDeleteAll(mChildren);
}

int QtSqlItem::type() const
{
    return mType;
}

QtSqlItem *QtSqlItem::parent() const
{
    return mParent;
}

QtSqlItem *QtSqlItem::childAt(int number)
{
    return mChildren.value(number);
}

int QtSqlItem::childCount() const
{
    return mChildren.size();
}

int QtSqlItem::childNumber() const
{
    if (mParent)
        return mParent->mChildren.indexOf(const_cast<QtSqlItem*>(this));

    return 0;
}

QString QtSqlItem::title() const
{
    return mTitle;
}

void QtSqlItem::setTitle(const QString &value)
{
    mTitle = value;
}

void QtSqlItem::appendChild(QtSqlItem *item)
{
    mChildren.append(item);
}

bool QtSqlItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > mChildren.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete mChildren.takeAt(position);

    return true;
}

bool QtSqlItem::removeChild(int position)
{
    return removeChildren(position, 1);
}



class QtSqlItemTreeModelPrivate
{
public:
    static const QLatin1String kRootItemName;

    QtSqlItemTreeModel::Categories categories;
    QScopedPointer<QtSqlItem> rootItem;
    bool supressDefaultConnection;

    QtSqlItemTreeModelPrivate();

    QtSqlItem* decodePointer(const QModelIndex& index) const;

    void update(const QModelIndex& index, QtSqlItemTreeModel* model);

    void populateDatabase(QtSqlItem* parent, QSqlDatabase db);
    void populateTable(QtSqlItem* parent, const QString& tableName, int type, const QSqlRecord& record);
};

const QLatin1String QtSqlItemTreeModelPrivate::kRootItemName("{root}");

QtSqlItemTreeModelPrivate::QtSqlItemTreeModelPrivate()
    : categories(QtSqlItemTreeModel::AllCategories)
    , rootItem(new QtSqlItem(-1, kRootItemName))
{}

QtSqlItem *QtSqlItemTreeModelPrivate::decodePointer(const QModelIndex &index) const
{
    if (index.isValid()) {
        QtSqlItem *item = static_cast<QtSqlItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem.data();
}

void QtSqlItemTreeModelPrivate::update(const QModelIndex &index, QtSqlItemTreeModel *model)
{
    if (index.isValid())
    {
        QtSqlItem* item = decodePointer(index);
        model->beginRemoveRows(index, 0, item->childCount());
        item->removeChildren(0, item->childCount());
        model->endRemoveRows();

        QtSqlItem* parent = item->parent();
        QString title = item->title();
        int type = item->type();

        parent->removeChild(item->childNumber());

        switch(type) {
        case QtSqlItemTreeModel::Database:
            populateDatabase(parent, QSqlDatabase::database(title, false));
            break;
        case QtSqlItemTreeModel::SystemTable:
        case QtSqlItemTreeModel::UserTable:
        case QtSqlItemTreeModel::View:
            if (parent != Q_NULLPTR) {
                QSqlDatabase db = QSqlDatabase::database(parent->title(), false);
                populateTable(parent, title, type, db.record(title));
            }
            break;
        default:
            break;
        }

    } else {
        model->beginResetModel();
        rootItem.reset(new QtSqlItem(-1, kRootItemName));
        QStringList keys = QSqlDatabase::connectionNames();
        std::sort(keys.begin(), keys.end());
        for (auto it = keys.begin(); it != keys.end(); ++it) {
            populateDatabase(rootItem.data(), QSqlDatabase::database(*it));
        }
        model->endResetModel();
    }
}

void QtSqlItemTreeModelPrivate::populateDatabase(QtSqlItem *parent, QSqlDatabase db)
{
    if (!(categories & QtSqlItemTreeModel::Database))
        return;

    if (!db.isValid())
        return;

    if (supressDefaultConnection && db.connectionName() == QSqlDatabase::defaultConnection)
        return;

    bool wasClosed = false;
    if (!db.isOpen()) {
        wasClosed = true;
        if (!db.open()) {
            qWarning() << "Failed to open database" << db.connectionName();
            return;
        }
    }

    QtSqlItem* item = new QtSqlItem(QtSqlItemTreeModel::Database, db.connectionName(), parent);
    parent->appendChild(item);

    QStringList tables;
    if (categories & QtSqlItemTreeModel::SystemTable) {
        tables = db.tables(QSql::SystemTables);
        for (auto it = tables.begin(); it != tables.end(); ++it)
            populateTable(item, *it, QtSqlItemTreeModel::SystemTable, db.record(*it));
    }

    if (categories & QtSqlItemTreeModel::UserTable) {
        tables = db.tables(QSql::Tables);
        for (auto it = tables.begin(); it != tables.end(); ++it)
            populateTable(item, *it, QtSqlItemTreeModel::UserTable, db.record(*it));
    }

    if (categories & QtSqlItemTreeModel::View) {
        tables = db.tables(QSql::Views);
        for (auto it = tables.begin(); it != tables.end(); ++it)
            populateTable(item, *it, QtSqlItemTreeModel::View, db.record(*it));
    }

    if (wasClosed)
        db.close();
}

void QtSqlItemTreeModelPrivate::populateTable(QtSqlItem *parent,
                                          const QString &tableName, int type,
                                          const QSqlRecord &record)
{
    QtSqlItem* tableItem = new QtSqlItem(type, tableName, parent);
    parent->appendChild(tableItem);

    if (categories & QtSqlItemTreeModel::Field) {
        for (int i = 0; i < record.count(); i++) {
            QtSqlItem* item = new QtSqlItem(QtSqlItemTreeModel::Field, record.fieldName(i), tableItem);
            tableItem->appendChild(item);
            qApp->processEvents();
        }
    }
}


QtSqlItemTreeModel::QtSqlItemTreeModel(QObject *parent) :
    QAbstractItemModel(parent),
    d(new QtSqlItemTreeModelPrivate)
{
}

QtSqlItemTreeModel::~QtSqlItemTreeModel()
{
}

void QtSqlItemTreeModel::setCategories(QtSqlItemTreeModel::Categories categories)
{
    if (d->categories != categories) {
        d->categories = categories;
        refresh();
    }
}

QtSqlItemTreeModel::Categories QtSqlItemTreeModel::categories() const
{
    return d->categories;
}

void QtSqlItemTreeModel::setDefaultDatabaseSupressed(bool on)
{
    d->supressDefaultConnection = on;
}

bool QtSqlItemTreeModel::isDefaultDatabaseSupressed() const
{
    return d->supressDefaultConnection;
}

QVariant QtSqlItemTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return tr("Entity");
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex QtSqlItemTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    QtSqlItem *parentItem = d->decodePointer(parent);
    QtSqlItem *childItem = parentItem->childAt(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex QtSqlItemTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    QtSqlItem *childItem = d->decodePointer(index);
    QtSqlItem *parentItem = childItem->parent();

    if (parentItem == d->rootItem.data())
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int QtSqlItemTreeModel::rowCount(const QModelIndex &parent) const
{
    QtSqlItem *item = d->decodePointer(parent);
    return (item != Q_NULLPTR ? item->childCount() : 0);
}

int QtSqlItemTreeModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant QtSqlItemTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.column() != 0)
        return QVariant();

    QtSqlItem *item = d->decodePointer(index);
    if (item == Q_NULLPTR) {
        return QVariant();
    }

    switch(role) {
    case Qt::DisplayRole:
        return item->title();
    case ItemTypeRole:
        return item->type();
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags QtSqlItemTreeModel::flags(const QModelIndex &index) const
{
    return (index.isValid() ? Qt::ItemIsEnabled|Qt::ItemIsSelectable : QAbstractItemModel::flags(index));
}

void QtSqlItemTreeModel::refresh()
{
    d->update(QModelIndex(), this);
}

void QtSqlItemTreeModel::update(const QModelIndex &index)
{
    d->update(index, this);
}




