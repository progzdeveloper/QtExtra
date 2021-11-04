#ifndef QTVARIANTLISTMODEL_H
#define QTVARIANTLISTMODEL_H

#include <QAbstractListModel>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtVariantListModel :
        public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable NOTIFY editableChanged)

public:
    explicit QtVariantListModel(QObject *parent = Q_NULLPTR);
    QtVariantListModel(const QVariantList& list, QObject *parent = Q_NULLPTR);
    virtual ~QtVariantListModel();

    bool isEditable() const;

    void setValues(const QVariantList& list);
    QVariantList values() const;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

    // Move data:
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void clear();
    void setEditable(bool on);

Q_SIGNALS:
   void editableChanged(bool);

protected:
   virtual QVariant defaultValue() const;

private:
    QT_PIMPL(QtVariantListModel)
};

#endif // QTVARIANTLISTMODEL_H
