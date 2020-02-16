#include "qtstateindicator.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleOptionFrame>

#include <QtCore/QPropertyAnimation>

#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QGradient>

#include <QtGui/QResizeEvent>

class QtStateIndicatorPrivate
{
public:
    QScopedPointer<QPropertyAnimation> animation;
    QtStateIndicator* indicator;
    QColor color;
    qreal frameWidth;
    QFrame::Shadow shadow;
    QtStateIndicator::Shape shape;

    bool clickable : 1;

    QtStateIndicatorPrivate(QtStateIndicator *w);
    void render(QPainter* painter, const QPalette& palette, const QRect& rect);
    void drawShape(QPainter *painter, const QRect& rect);
    void drawFrame(QPainter *painter, const QPalette &palette, const QRect& rect);
    void drawDiamondFrame(QPainter *painter, const QPalette &palette, const QRect& rect);
    void drawRectFrame(QPainter *painter, const QPalette &palette, const QRect& rect);
    void drawRoundFrame(QPainter *painter, const QPalette &palette, const QRect& rect);
    void drawCircleFrame(QPainter *painter, const QPalette &palette, const QRect& rect);
};


QtStateIndicatorPrivate::QtStateIndicatorPrivate(QtStateIndicator* w) :
    animation(Q_NULLPTR),
    indicator(w),
    color(Qt::green),
    frameWidth(0),
    shadow(QFrame::Plain),
    shape(QtStateIndicator::Rect),
    clickable(true)
{
}

void QtStateIndicatorPrivate::render(QPainter *painter, const QPalette& palette, const QRect& rect)
{
    painter->setRenderHint(QPainter::Antialiasing);
    drawShape(painter, rect.adjusted(frameWidth, frameWidth, -frameWidth, -frameWidth));
    drawFrame(painter, palette, rect.adjusted(1, 1, -1, -1));
}

void QtStateIndicatorPrivate::drawShape(QPainter *painter, const QRect &rect)
{
    painter->setPen(Qt::NoPen);
    QColor c = indicator->isEnabled() ? color : color.darker(250);
    switch (shadow)
    {
        case QFrame::Plain:
            painter->setBrush(c);
            break;
        case QFrame::Raised: {
                QRadialGradient gradient(rect.center(), qMin(rect.height(), rect.width()) / 2);
                gradient.setColorAt(0, c.lighter(180));
                gradient.setColorAt(1, c);
                painter->setBrush(gradient);
            } break;
        case QFrame::Sunken: {
                QRadialGradient gradient(rect.center(), qMin(rect.height(), rect.width()) / 2);
                gradient.setColorAt(0, c.lighter(120));
                gradient.setColorAt(1, c.darker(120));
                painter->setBrush(gradient);
            } break;
        default:
            break;
    }

    switch (shape) {
        case QtStateIndicator::Diamond:
        {
            QPoint points[5] = {
                QPoint(rect.center().x() + 1, rect.y()),
                QPoint(rect.x() + rect.width(), rect.center().y()),
                QPoint(rect.center().x() + 1, rect.bottom()),
                QPoint(rect.x(), rect.center().y()),
                QPoint(rect.center().x() + 1, rect.y())
            };
            painter->drawPolygon(points, 5);
        } break;
        case QtStateIndicator::Rect:
            painter->drawRect(rect);
            break;
        case QtStateIndicator::Round:
            painter->drawRoundedRect(rect, rect.width()/8.0, rect.height()/8.0);
            break;
        case QtStateIndicator::Circle:
            painter->drawEllipse(rect);
            break;
    }
}

void QtStateIndicatorPrivate::drawFrame(QPainter *painter, const QPalette &palette, const QRect &rect)
{
    if (frameWidth == 0.0)
        return;

    QRect r = rect.adjusted(frameWidth, frameWidth, -frameWidth, -frameWidth);
    switch (shape) {
    case QtStateIndicator::Diamond:
        drawDiamondFrame(painter, palette, rect);
        break;
    case QtStateIndicator::Rect:
        drawRectFrame(painter, palette, rect);
        break;
    case QtStateIndicator::Round:
        drawRoundFrame(painter, palette, r);
        break;
    case QtStateIndicator::Circle:
        drawCircleFrame(painter, palette, r);
        break;
    default:
        break;
    }
}

void QtStateIndicatorPrivate::drawDiamondFrame(QPainter *painter, const QPalette& palette, const QRect &rect)
{
    QPen pen(Qt::SolidLine);
    pen.setWidthF(frameWidth);
    pen.setJoinStyle(Qt::MiterJoin);

    painter->setBrush(Qt::NoBrush);

    switch(shadow)
    {
        case QFrame::Plain:
        {
            QPoint points[5] = {
                QPoint(rect.center().x() + 1, rect.y()),
                QPoint(rect.x() + rect.width(), rect.center().y()),
                QPoint(rect.center().x() + 1, rect.bottom()),
                QPoint(rect.x(), rect.center().y()),
                QPoint(rect.center().x() + 1, rect.y())
            };
            pen.setBrush(palette.dark());
            painter->setPen(pen);
            painter->drawPolygon(points, 5);
            break;
        }
        case QFrame::Raised:
        {
            pen.setBrush(palette.dark());
            painter->setPen(pen);
            painter->drawLine(QPoint(rect.center().x() + 1, rect.y()),
                              QPoint(rect.x() + rect.width(), rect.center().y()));
            painter->drawLine(QPoint(rect.x() + rect.width(), rect.center().y()),
                              QPoint(rect.center().x() + 1, rect.bottom()));

            pen.setBrush(palette.light());
            painter->setPen(pen);
            painter->drawLine(QPoint(rect.center().x() + 1, rect.bottom()),
                              QPoint(rect.x(), rect.center().y()));
            painter->drawLine(QPoint(rect.x(), rect.center().y()),
                              QPoint(rect.center().x() + 1, rect.y()));
            break;
        }
        case QFrame::Sunken:
        {
            pen.setBrush(palette.light());
            painter->setPen(pen);
            painter->drawLine(QPoint(rect.center().x() + 1, rect.y()),
                              QPoint(rect.x() + rect.width(), rect.center().y()));
            painter->drawLine(QPoint(rect.x() + rect.width(), rect.center().y()),
                              QPoint(rect.center().x() + 1, rect.bottom()));

            pen.setBrush(palette.dark());
            painter->setPen(pen);
            painter->drawLine(QPoint(rect.center().x() + 1, rect.bottom()),
                              QPoint(rect.x(), rect.center().y()));
            painter->drawLine(QPoint(rect.x(), rect.center().y()),
                              QPoint(rect.center().x() + 1, rect.y()));
            break;
        }

    }


}

void QtStateIndicatorPrivate::drawRectFrame(QPainter *painter, const QPalette &palette, const QRect &rect)
{

    switch (shadow) {
        case QFrame::Plain:
        {
            QPen pen(palette.dark(), frameWidth);
            pen.setJoinStyle(Qt::MiterJoin);
            painter->setPen(pen);
            painter->drawRect(rect);
            break;
        }
        case QFrame::Raised:
        {
            QStyle* style = qApp->style();
            QStyleOptionFrame option;
            option.rect = rect;
            option.lineWidth = frameWidth;
            option.features = QStyleOptionFrame::Flat;
            option.frameShape = QFrame::Panel;
            option.state = QStyle::State_Active|QStyle::State_Raised;
            option.palette = palette;
            style->drawControl(QStyle::CE_ShapedFrame, &option, painter);
            break;
        }
        case QFrame::Sunken:
        {
            QStyle* style = qApp->style();
            QStyleOptionFrame option;
            option.rect = rect;
            option.lineWidth = frameWidth;
            option.features = QStyleOptionFrame::Flat;
            option.frameShape = QFrame::Panel;
            option.state = QStyle::State_Active|QStyle::State_Sunken;
            option.palette = palette;
            style->drawControl(QStyle::CE_ShapedFrame, &option, painter);
            break;
        }
        default:
            break;
    }
}

void QtStateIndicatorPrivate::drawRoundFrame(QPainter *painter, const QPalette &palette, const QRect &rect)
{
    switch (shadow) {
        case QFrame::Plain:
        {
            painter->setPen(QPen(palette.dark(), frameWidth));
            painter->drawRoundedRect(rect, rect.width()/8.0, rect.height()/8.0);
            break;
        }
        case QFrame::Raised:
        {
            QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
            gradient.setColorAt(0.0, palette.light().color());
            gradient.setColorAt(1.0, palette.dark().color());
            painter->setPen(QPen(gradient, frameWidth));
            painter->drawRoundedRect(rect, rect.width()/8.0, rect.height()/8.0);
            break;
        }
        case QFrame::Sunken:
        {
            QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
            gradient.setColorAt(0.0, palette.dark().color());
            gradient.setColorAt(1.0, palette.light().color());
            painter->setPen(QPen(gradient, frameWidth));
            painter->drawRoundedRect(rect, rect.width()/8.0, rect.height()/8.0);
            break;
        }
        default:
            break;
    }
}

void QtStateIndicatorPrivate::drawCircleFrame(QPainter *painter, const QPalette &palette, const QRect &rect)
{
    switch (shadow) {
        case QFrame::Plain:
        {
            painter->setPen(QPen(palette.dark(), frameWidth));
            break;
        }
        case QFrame::Raised:
        {
            QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
            gradient.setColorAt(0.0, palette.light().color());
            gradient.setColorAt(1.0, palette.dark().color());
            painter->setPen(QPen(gradient, frameWidth));
            break;
        }
        case QFrame::Sunken:
        {
            QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
            gradient.setColorAt(0.0, palette.dark().color());
            gradient.setColorAt(1.0, palette.light().color());
            painter->setPen(QPen(gradient, frameWidth));
            break;
        }
        default:
            break;
    }
    painter->drawEllipse(rect);
}






QtStateIndicator::QtStateIndicator(QWidget *parent) :
    QAbstractButton(parent), d_ptr(new QtStateIndicatorPrivate(this))
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    resize(minimumSizeHint());
    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggle(bool)));
}

QtStateIndicator::QtStateIndicator(const QColor &color, Shape shape, QWidget *parent) :
    QAbstractButton(parent), d_ptr(new QtStateIndicatorPrivate(this))
{
    setColor(color);
    setShape(shape);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    resize(minimumSizeHint());
}

QtStateIndicator::~QtStateIndicator()
{
}

QSize QtStateIndicator::minimumSizeHint() const
{
    static QSize minSize(8+frameWidth(), 8+frameWidth());
    return minSize.expandedTo(QApplication::globalStrut());
}

int QtStateIndicator::heightForWidth(int w) const
{
    return w;
}

void QtStateIndicator::setShape(QtStateIndicator::Shape s)
{
    Q_D(QtStateIndicator);
    if (d->shape != s) {
        d->shape = s;
        update();
        Q_EMIT shapeChanged(s);
    }
}

QtStateIndicator::Shape QtStateIndicator::shape() const
{
    Q_D(const QtStateIndicator);
    return d->shape;
}

void QtStateIndicator::setColor(const QColor &c)
{
    Q_D(QtStateIndicator);
    if (d->color == c)
        return; // nothing to change

    if (d->animation) // animation is enabled
    {
        switch(d->animation->state())
        {
        case QPropertyAnimation::Stopped: // start animation is it is not already running
            // reset animation properties
            d->animation->setStartValue(d->color);
            d->animation->setEndValue(c);
            d->animation->start();
            Q_EMIT colorChanged(c);
            break;
        case QPropertyAnimation::Running:
            // only reset color value and repaint if animation has been already running
            d->color = c;
            update();
            break;
        default:
            break;
        }
        return;
    }

    // simply reset color value
    d->color = c;
    update();
    Q_EMIT colorChanged(c);
}

QColor QtStateIndicator::color() const
{
    Q_D(const QtStateIndicator);
    return d->color;
}

void QtStateIndicator::setClickable(bool on)
{
    Q_D(QtStateIndicator);
    if (d->clickable != on) {
        d->clickable = on;
        Q_EMIT clickableChanged(on);
    }
}

bool QtStateIndicator::isClickable() const
{
    Q_D(const QtStateIndicator);
    return d->clickable;
}

void QtStateIndicator::setAnimated(bool on)
{
    Q_D(QtStateIndicator);
    if (on) {
        if (d->animation != Q_NULLPTR) {
            return;
        }
        d->animation.reset(new QPropertyAnimation(this, "color", this));
    } else {
        d->animation->stop();
        d->animation.reset(Q_NULLPTR);
    }
}

bool QtStateIndicator::isAnimated() const
{
    Q_D(const QtStateIndicator);
    return (d->animation != Q_NULLPTR);
}

void QtStateIndicator::setDuration(int msec)
{
    Q_D(QtStateIndicator);
    if (d->animation != Q_NULLPTR) {
        if (d->animation->duration() != msec) {
            d->animation->setDuration(msec);
            Q_EMIT durationChanged(msec);
        }
    }
}

int QtStateIndicator::duration() const
{
    Q_D(const QtStateIndicator);
    return (d->animation != Q_NULLPTR ? d->animation->duration() : -1);
}

void QtStateIndicator::setFrameWidth(qreal w)
{
    Q_D(QtStateIndicator);
    if (d->frameWidth != w) {
        d->frameWidth = w;
        update();
        Q_EMIT frameWidthChanged(w);
    }
}

qreal QtStateIndicator::frameWidth() const
{
    Q_D(const QtStateIndicator);
    return d->frameWidth;
}

void QtStateIndicator::setShadow(QFrame::Shadow shadow)
{
    Q_D(QtStateIndicator);
    if (d->shadow != shadow) {
        d->shadow = shadow;
        update();
        Q_EMIT shadowChanged(shadow);
    }
}

QFrame::Shadow QtStateIndicator::shadow() const
{
    Q_D(const QtStateIndicator);
    return d->shadow;
}

void QtStateIndicator::timerEvent(QTimerEvent *event)
{
    QAbstractButton::timerEvent(event);
}

void QtStateIndicator::mousePressEvent(QMouseEvent *event)
{
    Q_D(QtStateIndicator);
    if (d->clickable)
        return QAbstractButton::mousePressEvent(event);
}

void QtStateIndicator::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(QtStateIndicator);
    if (d->clickable)
        return QAbstractButton::mouseReleaseEvent(event);
}

void QtStateIndicator::keyPressEvent(QKeyEvent *event)
{
    Q_D(QtStateIndicator);
    if (d->clickable)
        return QAbstractButton::keyPressEvent(event);
}

void QtStateIndicator::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(QtStateIndicator);
    if (d->clickable)
        return QAbstractButton::keyReleaseEvent(event);
}

void QtStateIndicator::paintEvent(QPaintEvent *)
{
    Q_D(QtStateIndicator);

    QPainter painter(this);
    d->render(&painter, palette(), rect());

}

void QtStateIndicator::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (width()!=height()) {
        int s = qMin(event->size().width(), event->size().height());
        resize(s, s);
        update();
        qApp->processEvents();
        return;
    }
    update();
}


bool QtStateIndicator::hitButton(const QPoint &pos) const
{
    return rect().contains(pos);
}

