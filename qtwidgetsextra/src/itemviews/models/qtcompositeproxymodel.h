#ifndef QTCOMPOSITEPROXYMODEL_H
#define QTCOMPOSITEPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QtWidgetsExtra>

class QtItemMapper;

class QTWIDGETSEXTRA_EXPORT QtCompositeProxyModel :
        public QIdentityProxyModel
{
    Q_OBJECT

public:
    enum BackgroundCombiation
    {
        Gradient  = 0x1,
        Mixed     = 0x2
    };

    enum TraverseOptions {
        TopMost   = 0x1,
        Forward   = 0x2,
        Backward  = 0x4,
        Propagate = 0x8,
        PropagateForward  = Propagate|Forward,
        PropagateBackward = Propagate|Backward,
    };

    explicit QtCompositeProxyModel(QObject* parent = Q_NULLPTR);
    ~QtCompositeProxyModel();


    void setTraverseOptions(int role, TraverseOptions opts);
    TraverseOptions traverseOptions(int role) const;

    void attachMapping(int sourceColumn, int sourceRole, QtItemMapper* mapper);
    int detachMapping(int sourceColumn, int sourceRole, QtItemMapper* mapper);
    int detachMappings(int sourceColumn, int sourceRole);
    int detachMapping(QtItemMapper* mapper);
    int countMappings(int sourceColumn, int sourceRole) const;
    void clearMappings();

    // QAbstractItemModel interface
public:
    virtual QVariant data(const QModelIndex &proxyIndex, int role) const Q_DECL_OVERRIDE;

private:
    QScopedPointer<class QtCompositeProxyModelPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtCompositeProxyModel)

};

#endif // QTCOMPOSITEPROXYMODEL_H
