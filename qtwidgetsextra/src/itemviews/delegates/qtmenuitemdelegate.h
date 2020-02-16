#ifndef QTMENUITEMDELEGATE_H
#define QTMENUITEMDELEGATE_H

#include <QtWidgetsExtra>
#include <QStyledItemDelegate>
#include <QItemDelegate>

class QTWIDGETSEXTRA_EXPORT QtMenuItemDelegate :
        public QItemDelegate
{
public:
    QtMenuItemDelegate(QObject* parent = Q_NULLPTR);

    // QAbstractItemDelegate interface
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // QTMENUITEMDELEGATE_H
