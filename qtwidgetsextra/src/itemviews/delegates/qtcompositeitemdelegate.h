#ifndef QTCOMPOSITEITEMDELEGATE_H
#define QTCOMPOSITEITEMDELEGATE_H

#include <QtWidgetsExtra>
#include <QStyledItemDelegate>

class QTWIDGETSEXTRA_EXPORT QtCompositeItemDelegate :
        public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QtCompositeItemDelegate(QObject* parent = Q_NULLPTR);
    virtual ~QtCompositeItemDelegate();

    // QAbstractItemDelegate interface
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // QTCOMPOSITEITEMDELEGATE_H
