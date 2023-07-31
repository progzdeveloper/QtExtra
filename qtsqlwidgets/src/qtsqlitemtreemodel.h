#ifndef QTSQLTREEMODEL_H
#define QTSQLTREEMODEL_H

#include <QAbstractItemModel>
#include <QtSqlWidgets>

class QTSQLWIDGETS_EXPORT QtSqlItemTreeModel :
        public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Category
    {
        Database    = 0x1,
        SystemTable = 0x2,
        UserTable   = 0x4,
        View        = 0x8,
        Field       = 0x10,
        AllCategories = Database|SystemTable|UserTable|View|Field
    };
    Q_DECLARE_FLAGS(Categories, Category)

    enum ExtendedRoles {
        ItemTypeRole = Qt::UserRole
    };

    explicit QtSqlItemTreeModel(QObject *parent = nullptr);
    ~QtSqlItemTreeModel();

    void setCategories(Categories categories);
    Categories categories() const;

    void setDefaultDatabaseSupressed(bool on);
    bool isDefaultDatabaseSupressed() const;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void refresh();
    virtual void update(const QModelIndex& index);

private:
    friend class QtSqlItemTreeModelPrivate;
    QScopedPointer<class QtSqlItemTreeModelPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QtSqlItemTreeModel::Categories)

#endif // QTSQLTREEMODEL_H
