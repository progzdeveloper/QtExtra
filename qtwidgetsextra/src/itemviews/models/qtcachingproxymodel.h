#ifndef QTCACHINGPROXYMODEL_H
#define QTCACHINGPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtCachingProxyModel :
        public QIdentityProxyModel
{
    Q_OBJECT

public:
    enum CachingPolicy
    {
        ManualUpdate = 0,
        AutoUpdate = 1
    };

    explicit QtCachingProxyModel(QObject* parent = Q_NULLPTR);
    ~QtCachingProxyModel();

    void setCachingPolicy(CachingPolicy policy);
    CachingPolicy cachingPolicy() const;

    void setUpdateInterval(int interval);
    int updateInterval() const;

    void setMaxCacheSize(int maxSize);
    int maxCacheSize() const;

    int cacheSize() const;

    void setCachedColumn(int column);
    int cachedColumn() const;

    void setCachedRoles(const std::vector<int>& roles);
    const std::vector<int>& cachedRoles() const;

    void setSourceModel(QAbstractItemModel* model) Q_DECL_OVERRIDE;

    bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &proxyIndex, int role) const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void clearCache();
    virtual void updateCache();
    virtual void cacheIndex(const QModelIndex& index);

protected:
    void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtCachingProxyModel)
};

#endif // QTCACHINGPROXYMODEL_H
