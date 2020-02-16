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

    QString text = index.data(Qt::DisplayRole).toString();

    QStyleOptionViewItem option = opt;
    initStyleOption(&option, index);
    option.text.clear();

    auto it = children.begin();
    for (auto e = children.end() - 1; it != e; ++it) {
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
    static QPoint zero;
    QRect rect(zero, QStyledItemDelegate::sizeHint(option, index));
    QList<QAbstractItemDelegate*> children = this->findChildren<QAbstractItemDelegate*>();
    for (auto it = children.begin(); it != children.end(); ++it) {
        rect |= (QRect(zero, (*it)->sizeHint(option, index)));
    }
    return rect.size();
}

