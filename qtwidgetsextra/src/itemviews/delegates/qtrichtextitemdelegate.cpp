#include <climits>
#include <float.h>

#include <QApplication>

#include <QHeaderView>
#include <QTreeView>
#include <QTableView>

#include <QTextEdit>
#include <QTextOption>
#include <QTextLayout>
#include <QStaticText>

#include <QColor>
#include <QPainter>

#include "qtrichtextitemdelegate.h"


class QtRichTextItemDelegatePrivate
{
public:
    //mutable QHeaderView* headerView;
    Qt::TextFormat format;
    mutable QTextDocument document;

    QtRichTextItemDelegatePrivate();
};

QtRichTextItemDelegatePrivate::QtRichTextItemDelegatePrivate() :
    // headerView(Q_NULLPTR)
    format(Qt::AutoText)
{
}


QtRichTextItemDelegate::QtRichTextItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent), d_ptr(new QtRichTextItemDelegatePrivate)
{
}

QtRichTextItemDelegate::~QtRichTextItemDelegate()
{
}

void QtRichTextItemDelegate::setTextFormat(Qt::TextFormat format)
{
    Q_D(QtRichTextItemDelegate);
    if (d->format == format)
        return;
    d->format = format;
    Q_EMIT textFormatChanged(format);
}

Qt::TextFormat QtRichTextItemDelegate::textFormat() const
{
    Q_D(const QtRichTextItemDelegate);
    return d->format;
}

void QtRichTextItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_D(const QtRichTextItemDelegate);
    if (!index.isValid() || d->format == Qt::PlainText) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QString text = option.text;//index.data(Qt::EditRole).toString();
    if (d->format == Qt::AutoText) {
        if (!Qt::mightBeRichText(text)) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }
    }

    QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();

    QStyleOptionViewItem options = option;
    //initStyleOption(&options, index);

    QStyle *style = option.widget? option.widget->style() : qApp->style();

    d->document.setDefaultFont(option.font);
    d->document.setDocumentMargin(0);
    d->document.setTextWidth(option.rect.width());
    d->document.setHtml(text);

    options.text = "";


    painter->save();

    style->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    // shift text right to make icon visible
    QSize iconSize;
    if (!icon.isNull()) {
         iconSize = icon.actualSize(options.rect.size());
    }

    painter->translate(options.rect.left() + 2 + iconSize.width(), options.rect.top() + 1);
    QRect clip(0, 0, options.rect.width() - iconSize.width(), options.rect.height());
    d->document.drawContents(painter, clip);
    painter->restore();

#if 0
    QStyleOptionViewItem options(option);
    initStyleOption(&options, index);
    options.text.clear();

    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    /*if (headerView == Q_NULLPTR) {
        const QTreeView* treeView = qobject_cast<const QTreeView*>(option.widget);
        if (treeView) {
            headerView = treeView->header();
            connect(headerView, SIGNAL(sectionResized(int,int,int)), treeView, SLOT(doItemsLayout()));
        }
        const QTableView* tableView = qobject_cast<const QTableView*>(option.widget);
        if (tableView) {
            headerView = tableView->horizontalHeader();
            connect(headerView, SIGNAL(sectionResized(int,int,int)), tableView, SLOT(doItemsLayout()));
        }
    }*/

    // shift text right to make icon visible
    /*QSize iconSize = options.icon.actualSize(options.rect.size());
    painter->translate(options.rect.left()+iconSize.width(), options.rect.top());

    QRect clip(0, 0, options.rect.width()+iconSize.width(), options.rect.height());
    painter->setClipRect(clip);*/

    QTextOption textOption(Qt::AlignLeft);
    textOption.setWrapMode(QTextOption::WordWrap);

    QStaticText staticText;
    staticText.setTextWidth(option.rect.width());
    staticText.setTextFormat(Qt::RichText);
    staticText.setTextOption(textOption);
    staticText.setText(index.data(Qt::EditRole).toString());

    int fontHeight = options.fontMetrics.height();
    int textHeight = staticText.size().height();
    int x = options.rect.x() + 2;
    int y = textHeight > fontHeight ? options.rect.y() + 1 : options.rect.center().y() - fontHeight/2;

    painter->save();
    style->drawControl(QStyle::CE_ItemViewItem, &options, painter);
    painter->setFont(options.font);
    if (option.state & QStyle::State_Selected)
        painter->setPen(option.palette.highlightedText().color());
    else
        painter->setPen(option.palette.text().color());
    painter->drawStaticText(x,y, staticText);
    painter->restore();

#endif
}

QSize QtRichTextItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid())
        return qvariant_cast<QSize>(value);

    QStyleOptionViewItem options(option);
    initStyleOption(&options, index);

    QTextOption textOption(Qt::AlignLeft);
    textOption.setWrapMode(QTextOption::WordWrap);

    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    QSize itemSize = style->sizeFromContents(QStyle::CT_ItemViewItem, &option, QSize(), option.widget);

    QString text = index.data(Qt::EditRole).toString();

    QStaticText staticText;
    staticText.setTextWidth(option.rect.width());
    staticText.setTextOption(textOption);
    staticText.setText(text);
    QSize textSize = staticText.size().toSize();
    return QSize(itemSize.width() + 2, textSize.height() - option.fontMetrics.height() > 2 ? textSize.height() : option.fontMetrics.height()+2);
}

/*
class HTMLDelegate : public QStyledItemDelegate
{
protected:
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

void HTMLDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    // shift text right to make icon visible
    QSize iconSize = options.icon.actualSize(options.rect.size());
    painter->translate(options.rect.left()+iconSize.width(), options.rect.top());
    QRect clip(0, 0, options.rect.width()+iconSize.width(), options.rect.height());

    //doc.drawContents(painter, clip);

    painter->setClipRect(clip);
    QAbstractTextDocumentLayout::PaintContext ctx;
    // set text color to red for selected item
    if (option.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, QColor("red"));
    ctx.clip = clip;
    doc.documentLayout()->draw(painter, ctx);

    painter->restore();
}

QSize HTMLDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}
*/



QWidget *QtRichTextItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    return new QTextEdit(parent);
}

void QtRichTextItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    QTextEdit* textEditor = qobject_cast<QTextEdit*>(editor);
    if (textEditor)
        textEditor->setHtml(index.data(Qt::EditRole).toString());
}

void QtRichTextItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QTextEdit* textEditor = qobject_cast<QTextEdit*>(editor);
    if (textEditor) {
        model->setData(index, textEditor->toHtml(), Qt::EditRole);
    }
}

void QtRichTextItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    int height = qMax(64, option.rect.height());
    int width = qMax(96, option.rect.width());
    QRect r(option.rect.x(), option.rect.y(), width, height);
    editor->setGeometry(r);
}

