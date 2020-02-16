#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QSize>
#include <QLinearGradient>
#include <QTextOption>
#include <QTextLayout>
#include <QStaticText>
#include <QStyle>
#include <QApplication>

#include <QTreeView>

#include "qtsheetitemdelegate.h"
#include "qtcompositeitemdelegate.h"

QtSheetItemDelegate::QtSheetItemDelegate(QTreeView *view, QObject *parent) :
    QStyledItemDelegate(parent),
    mView(view)
{
}

QtSheetItemDelegate::~QtSheetItemDelegate()
{
}

void QtSheetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    const QAbstractItemModel *model = index.model();
    Q_ASSERT(model);

    if (model->parent(index).isValid()) {
        if (qobject_cast<QtCompositeItemDelegate*>(parent()) == Q_NULLPTR)
            QStyledItemDelegate::paint(painter, opt, index);
        return;
    }

    QStyleOptionViewItem option = opt;
    //initStyleOption(&option, index);

    // this is a top-level item.
    QStyleOptionButton buttonOption;

    buttonOption.state = option.state;
#ifdef Q_OS_MACOS
    buttonOption.state |= QStyle::State_Raised;
#endif
    buttonOption.state &= ~QStyle::State_HasFocus;

    buttonOption.rect = option.rect;
    buttonOption.palette = option.palette;
    buttonOption.features = QStyleOptionButton::None;

    QColor buttonColor(230, 230, 230);
    QBrush buttonBrush = option.palette.button();
    if (!buttonBrush.gradient() && buttonBrush.texture().isNull())
        buttonColor = buttonBrush.color();
    QColor outlineColor = buttonColor.darker(150);
    QColor highlightColor = buttonColor.lighter(130);

    // Only draw topline if the previous item is expanded
    //QModelIndex previousIndex = model->index(index.row() - 1, index.column());
    bool drawTopline = false;//(index.row() > 0 && mView->isExpanded(previousIndex));
    int highlightOffset = 0;//drawTopline ? 1 : 0;

    QLinearGradient gradient(option.rect.topLeft(), option.rect.bottomLeft());
    gradient.setColorAt(0, buttonColor.lighter(102));
    gradient.setColorAt(1, buttonColor.darker(106));

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(gradient);
    painter->drawRect(option.rect);
    painter->setPen(highlightColor);
    painter->drawLine(option.rect.topLeft() + QPoint(0, highlightOffset),
                      option.rect.topRight() + QPoint(0, highlightOffset));
    painter->setPen(outlineColor);
    if (drawTopline)
        painter->drawLine(option.rect.topLeft(), option.rect.topRight());
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    painter->restore();


    QStyleOption branchOption;
    QRect r = option.rect;
    branchOption.rect = QRect(r.left() + 2, r.top() + r.height() / 2 - 8, 16, 16);  //QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
    branchOption.palette = option.palette;
    branchOption.state = QStyle::State_Children | option.state;

    if (mView->isExpanded(index))
        branchOption.state |= QStyle::State_Open;

    if (model->hasChildren(index) && index.column() == 0)
        mView->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, mView);

    QVariant value = model->data(index, Qt::DisplayRole);
    // draw text
    int i = 8;
    QRect textRect = QRect(r.left() + i*3, r.top() + r.height() / 2 - option.fontMetrics.height() / 2, r.width() - ((5*i)/2), r.height());
    QString text = elidedText(option.fontMetrics, textRect.width(), Qt::ElideRight, option.text/*value.toString()*/);

    value = model->data(index, Qt::TextAlignmentRole);
    Qt::Alignment textAlign = (value.isValid() ? static_cast<Qt::Alignment>(value.toInt()) :  Qt::AlignLeft|Qt::AlignVCenter);

    QTextOption textOption(textAlign);
    textOption.setWrapMode(QTextOption::WordWrap);

    QStaticText staticText;
    staticText.setTextOption(textOption);
    staticText.setTextWidth(option.rect.width());
    staticText.setText(text);

    painter->setPen(option.palette.text().color());
    painter->drawStaticText(textRect.topLeft(), staticText);
    /*mView->style()->drawItemText(painter, textrect, textAlign,
            option.palette, mView->isEnabled(), text);*/


}

QSize QtSheetItemDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    if (index.isValid() && !index.parent().isValid()) {
        QStyleOptionViewItem option = opt;
        return QStyledItemDelegate::sizeHint(opt, index) + QSize(2, 8);
    } else {
        return QStyledItemDelegate::sizeHint(opt, index);
    }
}




