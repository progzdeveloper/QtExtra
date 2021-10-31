#ifndef QTREVERTIBLEPROXYMODEL_H
#define QTREVERTIBLEPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtRevertibleProxyModel :
        public QIdentityProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool hasUncommitedChanges READ hasUncommitedChanges)
    Q_PROPERTY(int cacheSize READ cacheSize)
    Q_DISABLE_COPY(QtRevertibleProxyModel)

public:
    explicit QtRevertibleProxyModel(QObject *parent = Q_NULLPTR);
    virtual ~QtRevertibleProxyModel();

    bool hasUncommitedChanges() const;

    int cacheSize() const;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

    // QAbstractProxyModel interface
    virtual void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE;

    // QAbstractItemModel interface
public Q_SLOTS:
    virtual bool submit() Q_DECL_OVERRIDE;
    virtual void revert() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void changesCached(const QModelIndex&, int);
    void accepted();
    void rejected();

private:
    QT_PIMPL(QtRevertibleProxyModel)
};

#endif // QTREVERTIBLEPROXYMODEL_H
