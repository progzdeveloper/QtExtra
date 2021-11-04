#ifndef QTCHECKABLEPROXYMODEL_H
#define QTCHECKABLEPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtCheckableProxyModel :
        public QSortFilterProxyModel
{
    typedef QSortFilterProxyModel BaseModel;

    Q_OBJECT
public:
    explicit QtCheckableProxyModel(QObject *parent = Q_NULLPTR);
    ~QtCheckableProxyModel();

    // QAbstractProxyModel interface
    virtual void setSourceModel(QAbstractItemModel *model);

    void setModelColumn(int column);
    int modelColumn() const;

    void setCheckState(Qt::CheckState state);

    Qt::CheckState checkState() const;

    // QAbstractItemModel interface
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    // QAbstractItemModel interface
    virtual bool insertRows(int row, int count, const QModelIndex &parent);
    virtual bool removeRows(int row, int count, const QModelIndex &parent);

public Q_SLOTS:
    void setChecked();
    void setUnchecked();
    void invert();

    void filterChecked();
    void clearFilter();

    void insert(const QModelIndex&, int row, int count);
    void remove(const QModelIndex&, int row, int count);
    void resetInternals();

Q_SIGNALS:
    void checkStateChanged(Qt::CheckState);

private:
    QT_PIMPL(QtCheckableProxyModel)
};

#endif // QTCHECKABLEPROXYMODEL_H
