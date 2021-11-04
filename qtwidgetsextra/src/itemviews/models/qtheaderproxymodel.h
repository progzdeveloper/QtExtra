#ifndef QTHEADERPROXYMODEL_H
#define QTHEADERPROXYMODEL_H

#include <QtWidgetsExtra>
#include <QIdentityProxyModel>

class QTWIDGETSEXTRA_EXPORT QtHeaderProxyModel :
        public QIdentityProxyModel
{
    Q_OBJECT

public:
    explicit QtHeaderProxyModel(QObject *parent = Q_NULLPTR);
    ~QtHeaderProxyModel();

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

#endif // QTHEADERPROXYMODEL_H
