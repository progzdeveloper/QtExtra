#ifndef QTENUMLISTMODEL_H
#define QTENUMLISTMODEL_H

#include <QtWidgetsExtra>
#include <QAbstractListModel>
#include <QMetaEnum>


class QTWIDGETSEXTRA_EXPORT QtEnumListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit QtEnumListModel(QObject* parent = Q_NULLPTR);
    QtEnumListModel(const QMetaEnum& e, QObject* parent = Q_NULLPTR);

    ~QtEnumListModel();

    void setEnum(const QMetaEnum& e);

    QMetaEnum metaEnum() const;

    void setCheckable(bool on);
    bool isCheckable() const;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    // QAbstractItemModel interface
public Q_SLOTS:
    virtual void revert() Q_DECL_OVERRIDE;

    void clear();


protected:
    void setMeta(const QMetaEnum& e);

private:
    class QtEnumListModelPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QtEnumListModel)


};

#endif
