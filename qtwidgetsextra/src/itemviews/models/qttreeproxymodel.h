#ifndef QTTREEPROXYMODEL_H
#define QTTREEPROXYMODEL_H

#include <QAbstractItemModel>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtTreeProxyModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int groupColumn READ groupColumn WRITE setGroupColumn NOTIFY groupColumnChanged)
    Q_PROPERTY(int groupNameRole READ groupNameRole WRITE setGroupNameRole NOTIFY groupNameRoleChanged)

public:
    enum class CachingPolicy
    {
        NoCaching,
        CacheIndexes
    };

    explicit QtTreeProxyModel(QObject* parent = Q_NULLPTR);
    ~QtTreeProxyModel();

    void setSourceModel(QAbstractItemModel* model);
    QAbstractItemModel* sourceModel() const;

    void setGroupping(int column, int role);

    void setGroupColumn(int column);
    int groupColumn() const;

    void setGroupNameRole(int role);
    int groupNameRole() const;

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &proxyIndex, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onSourceRowsInserted(const QModelIndex &parent, int first, int last);
    void onSourceRowsRemoved(const QModelIndex &parent, int first, int last);
    void onSourceDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &roles);
    void onSourceReset();
    void onSourceDestroyed();

Q_SIGNALS:
    void groupColumnChanged(int);
    void groupNameRoleChanged(int);

private:
    QT_PIMPL(QtTreeProxyModel)
};

#endif // QTTREEPROXYMODEL_H
