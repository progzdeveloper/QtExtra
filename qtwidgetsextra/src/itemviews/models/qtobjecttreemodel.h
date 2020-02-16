#ifndef QTOBJECTTREEMODEL_H
#define QTOBJECTTREEMODEL_H

#include <QAbstractItemModel>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable NOTIFY editableChanged)

public:
    explicit QtObjectTreeModel(QObject *root, QObject *parent = Q_NULLPTR);

    void setRootObject(QObject* root);
    QObject* rootObject() const;

    QObject* object(const QModelIndex& index) const;

    void setEditable(bool on = true);
    bool isEditable() const;


    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void editableChanged(bool);
    void rootObjectChanged(QObject*);

    // QObject interface
protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    QObject* mRoot;
    bool mEditable;


};

#endif // QTOBJECTTREEMODEL_H
