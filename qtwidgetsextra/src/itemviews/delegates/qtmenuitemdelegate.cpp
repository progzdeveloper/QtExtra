#include "qtmenuitemdelegate.h"
#include "qtcompositeitemdelegate.h"
#include <QAbstractItemView>
#include <QPainter>
#include <QStyleOptionMenuItem>
#include <QApplication>
#include <QStyle>
#include <QTextOption>
#include <QTextLayout>
#include <QStaticText>

#include <QVariant>
#include <QPixmap>
#include <QImage>
#include <QIcon>

int indexDepth(const QModelIndex& index)
{
    int depth = 0;
    const QAbstractItemModel *model = index.model();
    QModelIndex idx = index;
    for (int i = 0; i < model->rowCount(idx); i++) {
        for (; model->hasChildren(idx); ++depth) {
            idx = idx.child(i, 0);
        }
    }
    return depth;
}

QtMenuItemDelegate::QtMenuItemDelegate(QObject* parent) :
    QItemDelegate(parent)
{
}


void QtMenuItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool hasChildren = false;
    const QAbstractItemModel *model = index.model();
    if (index.isValid()) {
        hasChildren = model->hasChildren(index);
    }
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    QStyleOptionMenuItem menuItemOption;
    menuItemOption.initFrom(option.widget);
    menuItemOption.state = option.state;
    menuItemOption.checked = (option.checkState == Qt::Checked);
    menuItemOption.text = QString(); // index.data().toString();
    if (qobject_cast<QtCompositeItemDelegate*>(parent()) == Q_NULLPTR)
        menuItemOption.icon = option.icon;
    menuItemOption.menuRect = option.rect;
    menuItemOption.rect = option.rect;
    //menuItemOption.rect = option.rect;
    menuItemOption.font = option.font;
    menuItemOption.fontMetrics = option.fontMetrics;
    menuItemOption.styleObject = option.styleObject;

    if (!index.isValid()) {
        menuItemOption.menuItemType = QStyleOptionMenuItem::EmptyArea;
    }
    else {
        menuItemOption.menuItemType = hasChildren ? QStyleOptionMenuItem::SubMenu : QStyleOptionMenuItem::Normal;
    }

    QString text = index.data().toString();
    if (std::all_of(text.begin(), text.end(), [](const QChar& c) { return c == '-'; })) {
        menuItemOption.menuItemType = QStyleOptionMenuItem::Separator;
        text.clear();
    }

    /*if (childrenCount > 0) {
        QStyleOptionViewItem opt(option);
        if (!index.parent().isValid())
            style->drawPrimitive(QStyle::PE_FrameMenu, &opt, painter);

        opt.rect.setX(option.rect.x() + 16);
        opt.rect.setY(option.rect.y() + option.rect.height());
        opt.rect.setHeight(option.rect.height() * childrenCount);
        style->drawPrimitive(QStyle::PE_FrameMenu, &opt, painter);
    } */


    const QBrush& background = option.state & QStyle::State_Selected ?
                               option.palette.highlight() :
                               option.palette.background();
    painter->fillRect(menuItemOption.rect, background);

    painter->save();
    painter->setPen(option.palette.dark().color());
    if (hasChildren && !index.parent().isValid()) {
        painter->drawLine(menuItemOption.menuRect.topLeft(),
                          menuItemOption.menuRect.topRight());
        painter->drawLine(menuItemOption.menuRect.bottomLeft(),
                          menuItemOption.menuRect.bottomRight());
    }
    if (hasChildren ||
        (!hasChildren && index.parent().isValid() && index.row() == model->rowCount(index.parent())-1))
    {
        painter->drawLine(menuItemOption.menuRect.bottomLeft(),
                          menuItemOption.menuRect.bottomRight());
    }
    painter->setPen(option.palette.midlight().color());
    painter->drawLine(menuItemOption.menuRect.topLeft(), menuItemOption.menuRect.bottomLeft());
    painter->drawLine(menuItemOption.menuRect.topRight(), menuItemOption.menuRect.bottomRight());
    painter->restore();

    style->drawControl(QStyle::CE_MenuItem, &menuItemOption, painter);

    QTextOption textOption(Qt::AlignLeft);
    textOption.setWrapMode(QTextOption::NoWrap);
    QStaticText staticText;
    staticText.setTextWidth(menuItemOption.menuRect.width());
    staticText.setTextOption(textOption);
    staticText.setText(text);
    QPoint p(option.rect.topLeft());
    int w = style->pixelMetric(QStyle::PM_ToolBarIconSize) +
            style->pixelMetric(QStyle::PM_MenuButtonIndicator);
    p.rx() += w;
    p.ry() += 4;
    if (option.state & QStyle::State_Selected)
        painter->setPen(option.palette.highlightedText().color());
    else
        painter->setPen(option.palette.text().color());
    //painter->drawStaticText(p, staticText);
}

QSize QtMenuItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid())
        return qvariant_cast<QSize>(value);

    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    //QStyleOptionMenuItem menuItemOption;
    QSize size = option.fontMetrics.size(Qt::TextSingleLine, index.data().toString());
    size.rheight() = 24;
    size.rwidth() += style->pixelMetric(QStyle::PM_MenuButtonIndicator);
    return size;
}

