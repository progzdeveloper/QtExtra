#include "qtcompositeitemdelegate.h"
#include <QPainter>

QtCompositeItemDelegate::QtCompositeItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QtCompositeItemDelegate::~QtCompositeItemDelegate()
{
}

void QtCompositeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    QList<QAbstractItemDelegate*> children = this->findChildren<QAbstractItemDelegate*>();
    if (children.isEmpty()) {
        QStyledItemDelegate::paint(painter, opt, index);
        return;
    }

    const QString text = index.data(Qt::DisplayRole).toString();

    QStyleOptionViewItem option = opt;
    initStyleOption(&option, index);
    option.text.clear();

    auto it = children.cbegin();
    for (auto last = children.cend() - 1; it != last; ++it)
    {
        painter->save();
        (*it)->paint(painter, option, index);
        painter->restore();
    }
    option.text = text;
    painter->save();
    (*it)->paint(painter, option, index);
    painter->restore();
}

QSize QtCompositeItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    const QList<QAbstractItemDelegate*> children = this->findChildren<QAbstractItemDelegate*>();
    for (auto it = children.cbegin(); it != children.cend(); ++it)
        size = size.expandedTo((*it)->sizeHint(option, index));

    return size;
}

