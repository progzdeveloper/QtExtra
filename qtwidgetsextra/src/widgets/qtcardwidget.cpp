#include "qtcardwidget.h"
#include <QVariant>
#include <QPainter>
#include <QPainterPath>
#include <cstring>
#include <cstdlib>

void QtStyleOptionChip::paint(QPainter *painter, const QString &text) const
{
    int r = std::min(rect.width(), rect.height()) / 2;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(background);
    painter->drawRoundedRect(rect, r, r);
    painter->setFont(font);

    QRect textRect;

    if (!closeable)
        textRect = rect.adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom());
    else
        textRect = rect.adjusted(margins.left(), margins.top(), -(buttonSize.width()+margins.right()), -margins.bottom());

    painter->setPen(foreground);
    // draw text
    painter->drawText(textRect, text, textOptions);

    if (closeable) {
        painter->setBrush(Qt::NoBrush);
        // draw button
        painter->drawEllipse(rect.right() - margins.right() - buttonSize.width()/2, rect.y() + (rect.height() / 2 - buttonSize.height() / 2), buttonSize.width(), buttonSize.height());
    }
    painter->restore();
}

void QtStyleOptionChip::paint(QPainter *painter, const QPixmap &pixmap) const
{
    int r = std::min(rect.width()-margins.right(), rect.height()-margins.bottom());

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::NoPen);
    painter->setClipping(true);
    QRectF targetRect(rect.x() + margins.left(), rect.y() + margins.top(), r, r);

    QPainterPath path;
    path.addEllipse(targetRect);

    painter->setClipping(true);
    painter->setClipPath(path);

    painter->setPen(Qt::NoPen);
    painter->fillRect(targetRect, background);
    if (!pixmap.isNull())
        painter->drawPixmap(targetRect, pixmap, pixmap.rect());

    painter->setClipping(false);
    painter->restore();
}

void QtStyleOptionChip::paint(QPainter *painter, const QPixmap &pixmap, const QString &text) const
{
    paint(painter, pixmap);
    paint(painter, text);
}



class QtGraphicsBageEffectPrivate
{
public:
    QPixmap pixmap;
    QMargins margins;
    QSize maximumSize;
    char text[5];
    int value;
    Qt::Alignment alignment;
    QtStyleOptionChip styleOption;

    QtGraphicsBageEffectPrivate() :
        margins(4, 4, 4, 4),
        maximumSize(34, 16),
        value(-1),
        alignment(Qt::AlignTop|Qt::AlignLeft)
    {
        std::memset(text, 0, sizeof(text));
        styleOption.background = Qt::lightGray;
        styleOption.foreground = Qt::darkGray;
        styleOption.closeable = false;
        styleOption.textOptions.setAlignment(Qt::AlignCenter);
        styleOption.font.setFamily("Robato");
        styleOption.font.setPointSize(8);
    }
};


QtGraphicsBageEffect::QtGraphicsBageEffect(QObject *parent) :
    QGraphicsEffect(parent),
    d_ptr(new QtGraphicsBageEffectPrivate)
{
}

QtGraphicsBageEffect::~QtGraphicsBageEffect()
{
}

void QtGraphicsBageEffect::setMaximumSize(const QSize &size)
{
    Q_D(QtGraphicsBageEffect);
    if (d->maximumSize == size)
        return;

    d->maximumSize = size;
    update();
}

QSize QtGraphicsBageEffect::maximumSize() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->maximumSize;
}

void QtGraphicsBageEffect::setMargins(const QMargins &margins)
{
    Q_D(QtGraphicsBageEffect);
    if (d->margins == margins)
        return;

    d->margins = margins;
    update();
}

QMargins QtGraphicsBageEffect::margins() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->margins;
}

void QtGraphicsBageEffect::setBackground(const QColor &color)
{
    Q_D(QtGraphicsBageEffect);
    if (d->styleOption.background == color)
        return;

    d->styleOption.background = color;
    update();
}

QColor QtGraphicsBageEffect::background() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->styleOption.background;
}

void QtGraphicsBageEffect::setForeground(const QColor &color)
{
    Q_D(QtGraphicsBageEffect);
    if (d->styleOption.foreground == color)
        return;

    d->styleOption.foreground = color;
    update();
}

QColor QtGraphicsBageEffect::foreground() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->styleOption.foreground;
}

void QtGraphicsBageEffect::setFont(const QFont &font)
{
    Q_D(QtGraphicsBageEffect);
    if (d->styleOption.font == font)
        return;

    d->styleOption.font = font;
    update();
}

QFont QtGraphicsBageEffect::font() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->styleOption.font;
}

void QtGraphicsBageEffect::setCounter(int value)
{
    Q_D(QtGraphicsBageEffect);
    if (value == d->value)
        return;
    d->value = value;
    if (value < 0) {
        d->value = -1;
        std::memset(d->text, 0, sizeof(d->text));
    } else {
        if (d->value > 999)
            std::strncpy(d->text, "999+", sizeof("999+")-1);
        else
            std::snprintf(d->text, sizeof(d->text), "%i", d->value);
        d->pixmap = QPixmap();
    }
    update();
}

int QtGraphicsBageEffect::counter() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->value;
}

QString QtGraphicsBageEffect::text() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->text;
}

void QtGraphicsBageEffect::setIcon(const QPixmap &icon)
{
    Q_D(QtGraphicsBageEffect);
    if (icon.isNull())
        return;

    d->pixmap = icon;
    d->value = -1;
    std::memset(d->text, 0, sizeof(d->text));
    update();
}

QPixmap QtGraphicsBageEffect::icon() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->pixmap;
}

void QtGraphicsBageEffect::setValue(const QVariant &value)
{
    Q_D(QtGraphicsBageEffect);

    switch (value.type())
    {
    case QVariant::Pixmap:
        setIcon(value.value<QPixmap>());
        break;
    case QVariant::Image:
        setIcon(QPixmap::fromImage(value.value<QImage>()));
        break;
    case QVariant::Icon:
        setIcon(value.value<QIcon>().pixmap(d->maximumSize));
        break;
    case QVariant::Int:
        setCounter(value.toInt());
        break;
    default:
        d->pixmap = QPixmap();
        d->value = -1;
        std::memset(d->text, 0, sizeof(d->text));
        update();
        break;
    }
}

QVariant QtGraphicsBageEffect::value() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->pixmap.isNull() ? QVariant(d->value) : QVariant(d->pixmap);
}

void QtGraphicsBageEffect::setAlignment(Qt::Alignment align)
{
    Q_D(QtGraphicsBageEffect);
    if (d->alignment == align)
        return;
    d->alignment = align;
    update();
}

Qt::Alignment QtGraphicsBageEffect::alignment() const
{
    Q_D(const QtGraphicsBageEffect);
    return d->alignment;
}

void QtGraphicsBageEffect::draw(QPainter *painter)
{
    Q_D(QtGraphicsBageEffect);
    if (d->value < 1 && d->pixmap.isNull())
        return drawSource(painter);

    const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates);
    painter->drawPixmap(0, 0, pixmap);

    painter->save();
    QRectF rect = boundingRect();
    rect.adjust(d->margins.left(), d->margins.top(), -d->margins.right(), -d->margins.bottom());
    rect.setWidth(std::min(rect.width(), (qreal)d->maximumSize.width()));
    rect.setHeight(std::min(rect.height(), (qreal)d->maximumSize.height()));
    d->styleOption.rect = rect.toRect();
    if (d->value < 0)
        d->styleOption.paint(painter, d->pixmap);
    else
        d->styleOption.paint(painter, QString::fromLatin1(d->text));

    painter->restore();
}
