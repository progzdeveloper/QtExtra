#ifndef QTSUBSETPROXYMODEL_H
#define QTSUBSETPROXYMODEL_H

#include <QtWidgetsExtra>

#include <QIdentityProxyModel>

class QTWIDGETSEXTRA_EXPORT QtColumnProxyModel :
        public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit QtColumnProxyModel(QObject *parent = 0);
    ~QtColumnProxyModel();

    // API

    void setSourceColumns(const QVector<int> &columns);
    const QVector<int>& sourceColumns() const;

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    QModelIndex sibling(int row, int column, const QModelIndex &idx) const Q_DECL_OVERRIDE;

    // QAbstractProxyModel interface
public:
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;


private:
    QT_PIMPL(QtColumnProxyModel)
};

#endif // QTSUBSETPROXYMODEL_H
