#include <QApplication>
#include <QPainter>
#include <QLinearGradient>

#include "qtcounteritemdelegate.h"
#include "qtcompositeitemdelegate.h"

struct RatioSuffix
{
    qint64 amount;
    char suffix;
};


static const RatioSuffix ratios[] = {
    { 1000, 'K' }, // Kilo
    { 1000000, 'M' }, // Mega
    { 1000000000, 'G' }, // Giga
    { 1000000000000LL, 'T' }, // Tera
    { 1000000000000000LL, 'P' }, // Peta
    { 1000000000000000000LL, 'E' } // Exa
};


QtCounterItemDelegate::QtCounterItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent), mMode(Suffix), mIconSize(12, 12)
{
}

QtCounterItemDelegate::QtCounterItemDelegate(QtCounterItemDelegate::CounterMode mode, QObject *parent) :
    QStyledItemDelegate(parent), mMode(mode), mIconSize(12, 12)
{
}

QtCounterItemDelegate::~QtCounterItemDelegate()
{
}

void QtCounterItemDelegate::setCounterMode(QtCounterItemDelegate::CounterMode mode)
{
    mMode = mode;
}

QtCounterItemDelegate::CounterMode QtCounterItemDelegate::counterMode() const
{
    return mMode;
}

void QtCounterItemDelegate::setIconSize(const QSize &size)
{
    mIconSize = size;
}

QSize QtCounterItemDelegate::iconSize() const
{
    return mIconSize;
}

void QtCounterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QAbstractItemModel *model = index.model();
    Q_ASSERT(model);

    if (model->hasChildren(index) && index.column() == 0)
    {
        QRect cachedRect;
        QStyledItemDelegate::paint(painter, option, index);

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        opt.font.setPointSize(opt.font.pointSize() - 2);
        opt.fontMetrics = QFontMetrics(opt.font);
        int childrenCount = index.model()->rowCount(index);
        const QString text =  formatCounter(childrenCount);
        if (mMode == Plus) {
            cachedRect = opt.fontMetrics.boundingRect("999+");
        } else { // None or Suffix
            cachedRect = opt.fontMetrics.boundingRect(text);
        }

        cachedRect.setWidth(qMax(cachedRect.width() + 12, 24));

        const bool selected = option.state & QStyle::State_Selected;
        const QColor pillColor = option.palette.color(selected ? QPalette::Base : QPalette::Highlight);
        const QColor textColor = option.palette.color(selected ? QPalette::Text : QPalette::HighlightedText);

        QRect rc =  opt.rect.adjusted(option.rect.width() - cachedRect.width(), 2, -2, -2);
        rc.setSize(QSize(iconSize().width() + 5, iconSize().height()));
        rc.translate(-4, (option.rect.height() - rc.height()) / 2 - 1);

        const int radius = std::min(rc.height(), rc.width()) / 2;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(pillColor);
        painter->drawRoundedRect(rc, radius, radius);
        painter->setPen(textColor);
        painter->setFont(opt.font);
        painter->drawText(rc, Qt::AlignCenter, text);
        painter->restore();

    } else {
        if (qobject_cast<QtCompositeItemDelegate*>(parent()) == Q_NULLPTR)
            QStyledItemDelegate::paint(painter, option, index);
    }
}

QString QtCounterItemDelegate::formatCounter(qint64 count) const
{
    if (mMode == Plus && count > (ratios[0].amount - 1)) {
          return "999+";
    }

    if (mMode == Suffix)
    {
        int index = sizeof(ratios) / sizeof(ratios[0]) - 1;
        for (; index >= 0; --index) {
            double value = (double)count / ratios[index].amount;
            if (value >= 1.0) {
                return QString::number(value, 'f', 1) + ratios[index].suffix;
            }
        }
    }

    return QString::number(count);
}

QSize QtCounterItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    QRect defaultRect(QPoint(0, 0), QStyledItemDelegate::sizeHint(option, index));

    const QAbstractItemModel *model = index.model();
    Q_ASSERT(model);
    if (model->hasChildren(index) && index.column() == 0)
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        opt.font.setPointSize(opt.font.pointSize() - 2);
        opt.fontMetrics = QFontMetrics(opt.font);
        int childrenCount = index.model()->rowCount(index);
        const QString text =  formatCounter(childrenCount);

        QRect cachedRect;
        if (mMode == Plus) {
            cachedRect = opt.fontMetrics.boundingRect("999+");
        } else { // None or Suffix
            cachedRect = opt.fontMetrics.boundingRect(text);
        }
        cachedRect.setWidth(qMax(cachedRect.width() + 12, 28));
        defaultRect.adjust(0, 0, cachedRect.width(), 0);
    }
    return defaultRect.size();
}
