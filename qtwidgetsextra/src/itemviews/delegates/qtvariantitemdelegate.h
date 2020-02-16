#ifndef QTVARIANTITEMDELEGATE_H
#define QTVARIANTITEMDELEGATE_H

#include <QtWidgetsExtra>

#include <QVariant>
#include <QStyledItemDelegate>
#include <QItemEditorFactory>



class QWidget;
class QModelIndex;
class QStyleOptionViewItem;
class QAbstractItemModel;


class QTWIDGETSEXTRA_EXPORT QtVariantItemEditorFactory :
        public QItemEditorFactory
{
    Q_DISABLE_COPY(QtVariantItemEditorFactory)
public:
    QtVariantItemEditorFactory();
    ~QtVariantItemEditorFactory();
    QWidget *createEditor(int userType, QWidget * parent) const;
    QByteArray valuePropertyName(int userType) const;
    QVariant attribute(int userType, const char* key) const;
    void setAttribute(int userType, const char* key, const QVariant& value);
private:
    QT_PIMPL(QtVariantItemEditorFactory)
};



class QtVariantItemDelegatePrivate;
class QTWIDGETSEXTRA_EXPORT QtVariantItemDelegate :
        public QStyledItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY(QtVariantItemDelegate)
public:
    explicit QtVariantItemDelegate(QObject *parent = 0);
    ~QtVariantItemDelegate();

    void setItemRole(int role);
    int itemRole() const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

    // QAbstractItemDelegate interface
public:
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
private:
    QT_PIMPL(QtVariantItemDelegate)

};


#endif
