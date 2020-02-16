#include "qtprogressitemdelegate.h"
#include <QApplication>
#include <QtGui>

QtProgressItemDelegate::QtProgressItemDelegate(QObject * parent) :
    QItemDelegate(parent), mPrec(1)
{
}

QtProgressItemDelegate::~QtProgressItemDelegate()
{
}

void QtProgressItemDelegate::setPrecision(int prec)
{
    mPrec = prec;
}

int QtProgressItemDelegate::precision() const
{
    return mPrec;
}

QRect QtProgressItemDelegate::rect(const QStyleOptionViewItem &option,
                                   const QModelIndex &index, int role) const
{
    QVariant value = index.data(role);
    if (value.isValid() && !value.isNull()) {
        switch (value.type()) {
            case QVariant::Invalid:
                break;
            case QVariant::Pixmap:
                return QRect(QPoint(0, 0), qvariant_cast<QPixmap>(value).size());
            case QVariant::Image:
                return QRect(QPoint(0, 0), qvariant_cast<QImage>(value).size());
            case QVariant::Icon: {
                QIcon icon = qvariant_cast<QIcon>(value);
                QSize size = icon.actualSize(option.decorationSize);
                return QRect(QPoint(0, 0), size);
            }
            case QVariant::Color:
                return QRect(QPoint(0, 0), option.decorationSize);
            case QVariant::String:
            default: {
                QString text = index.data(Qt::DisplayRole).toString();
                value = index.data(Qt::FontRole);
                QFont fnt = qvariant_cast<QFont>(value).resolve(option.font);

                QTextOption textOption(Qt::AlignCenter);
                textOption.setWrapMode(QTextOption::WordWrap);

                QTextLayout textLayout(text);
                textLayout.setTextOption(textOption);
                textLayout.setFont(fnt);
                textLayout.setText(text);

                qreal height = 0;
                qreal widthUsed = 0;
                textLayout.beginLayout();
                while (true) {
                    QTextLine line = textLayout.createLine();
                    if (!line.isValid())
                        break;
                    line.setLineWidth(option.rect.width());
                    line.setPosition(QPointF(0, height));
                    height += line.height();
                    widthUsed = qMax(widthUsed, line.naturalTextWidth());
                }
                textLayout.endLayout();
                QSizeF size(widthUsed, height);
                const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
                return QRect(0, 0, size.width() + 2 * textMargin, size.height());
            }
        }
    }
    return QRect();
}

void QtProgressItemDelegate::paint(QPainter *painter, 
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    double progress = index.data(Qt::DisplayRole).toDouble();

    QStyleOptionProgressBar progressBarOption;
    progressBarOption.rect = option.rect.adjusted(1, 1, -1, -1);
    progressBarOption.minimum = 0;
    progressBarOption.maximum = 100;
    progressBarOption.progress = progress;
    progressBarOption.text = QString::number(progress, 'f', mPrec) + " %";
    progressBarOption.textVisible = true;
    progressBarOption.textAlignment = Qt::AlignCenter;
    progressBarOption.state = option.state;
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ProgressBar,
                                       &progressBarOption, painter);
}

QSize QtProgressItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid())
        return qvariant_cast<QSize>(value);

    QRect decorationRect = rect(option, index, Qt::DecorationRole);
    QRect displayRect = rect(option, index, Qt::DisplayRole);
    QRect checkRect = rect(option, index, Qt::CheckStateRole);

    return (decorationRect|displayRect|checkRect).size();
}
