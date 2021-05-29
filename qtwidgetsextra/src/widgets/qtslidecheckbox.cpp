#include "qtslidecheckbox.h"
#include <QTimeLine>
#include <QPainter>

class QtSlideCheckBoxPrivate
{
    static constexpr int kAnimationDuration = 95;
public:
    QtSlideCheckBox* q_ptr;

    QTimeLine* timeLine;
    QColor background[2];
    Qt::Orientation orientation;
    int spacing;
    qreal position;
    bool animated;

    QtSlideCheckBoxPrivate(Qt::Orientation o, QtSlideCheckBox* q) :
        q_ptr(q),
        timeLine(new QTimeLine(kAnimationDuration, q)),
        orientation(o),
        spacing(2),
        position(0),
        animated(true)
    {
        timeLine->setUpdateInterval(20); // ~50 FPS
        background[0] = Qt::gray;
        background[1] = Qt::green;
        QObject::connect(timeLine, &QTimeLine::valueChanged, q, &QtSlideCheckBox::animate);
    }

    inline uint interpolate(const uint f, const uint t, qreal progress)
    {
        return uint(f + t * progress - f * progress);
    }

    inline QColor interpolate(const QColor &f,const QColor &t, qreal progress)
    {
        return QColor(qBound(0U, interpolate(f.red(), t.red(), progress), 255U),
                      qBound(0U, interpolate(f.green(), t.green(), progress), 255U),
                      qBound(0U, interpolate(f.blue(), t.blue(), progress), 255U),
                      qBound(0U, interpolate(f.alpha(), t.alpha(), progress), 255U));
    }

    void drawBackground(QPainter& painter, const QSize& size, Qt::CheckState state)
    {
        QColor color;
        QRectF rect(0, 0, size.width() - 1, size.height() - 1);
        qreal r = (orientation == Qt::Horizontal ? ((size.height()-1)*0.5) : ((size.width()-1)*0.5));

        if (timeLine->state() == QTimeLine::Running)
            color = interpolate(background[0], background[1], position);
        else
            color = background[state == Qt::Checked];

        painter.setPen(Qt::NoPen/*Qt::gray*/);
        painter.setBrush(color);
        painter.drawRoundedRect(rect, r, r);
    }

    void drawPill(QPainter& painter, const QSize& size, bool enabled)
    {
        static const int margin = 2;
        painter.setPen(Qt::NoPen);
        painter.setBrush(enabled ? Qt::white : Qt::lightGray);
        qreal p = (orientation == Qt::Horizontal ? (size.width() - size.height()) : (size.height() - size.width()));
        qreal w = (orientation == Qt::Horizontal ? (size.height() - 2*margin) : (size.width() - 2*margin)) - 1;
        if (orientation == Qt::Horizontal)
            painter.drawEllipse(p*position + margin, margin, w, w);
        else
            painter.drawEllipse(margin, p - p*position + margin, w, w);
    }

    void updateSizePolicy()
    {
        if (orientation == Qt::Vertical)
            q_ptr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        else
            q_ptr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
};

QtSlideCheckBox::QtSlideCheckBox(QWidget *parent) :
    QCheckBox(parent),
    d_ptr(new QtSlideCheckBoxPrivate(Qt::Horizontal, this))
{
    Q_D(QtSlideCheckBox);
    d->updateSizePolicy();
}

QtSlideCheckBox::QtSlideCheckBox(Qt::Orientation orientation, QWidget *parent) :
    QCheckBox(parent),
    d_ptr(new QtSlideCheckBoxPrivate(orientation, this))
{
    Q_D(QtSlideCheckBox);
    d->updateSizePolicy();
}

QtSlideCheckBox::~QtSlideCheckBox()
{
}

void QtSlideCheckBox::setSpacing(int m)
{
    Q_D(QtSlideCheckBox);
    d->spacing = m;
    if (isVisible())
        update();
}

int QtSlideCheckBox::spacing() const
{
    Q_D(const QtSlideCheckBox);
    return d->spacing;
}

void QtSlideCheckBox::setAnimated(bool on)
{
    Q_D(QtSlideCheckBox);
    d->animated = on;
}

bool QtSlideCheckBox::isAnimated() const
{
    Q_D(const QtSlideCheckBox);
    return d->animated;
}

void QtSlideCheckBox::setDuration(int msec)
{
    Q_D(QtSlideCheckBox);
    d->timeLine->setDuration(msec);
}

int QtSlideCheckBox::duration() const
{
    Q_D(const QtSlideCheckBox);
    return d->timeLine->duration();
}

void QtSlideCheckBox::setOrientation(Qt::Orientation orientation)
{
    Q_D(QtSlideCheckBox);
    if (d->orientation == orientation)
        return;

    d->orientation = orientation;
    d->updateSizePolicy();
    if (isVisible())
        update();

    Q_EMIT orientationChanged(orientation);
}

Qt::Orientation QtSlideCheckBox::orientation() const
{
    Q_D(const QtSlideCheckBox);
    return d->orientation;
}

void QtSlideCheckBox::animate(qreal value)
{
    Q_D(QtSlideCheckBox);
    d->position = value;
    update();
}

QSize QtSlideCheckBox::minimumSizeHint() const
{
    Q_D(const QtSlideCheckBox);
    return (d->orientation == Qt::Horizontal ? QSize(28, 16) : QSize(16, 28));
    //return (d->orientation == Qt::Horizontal ? QSize(48, 32) : QSize(32, 48));
}

void QtSlideCheckBox::resizeEvent(QResizeEvent *e)
{
    QCheckBox::resizeEvent(e);
}

void QtSlideCheckBox::paintEvent(QPaintEvent *)
{
    Q_D(QtSlideCheckBox);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    d->drawBackground(painter, size(), checkState());
    d->drawPill(painter, size(), isEnabled());
}

bool QtSlideCheckBox::hitButton(const QPoint &pos) const
{
    return rect().contains(pos);
}

void QtSlideCheckBox::nextCheckState()
{
    Q_D(QtSlideCheckBox);
    if (d->animated && isVisible()) {
        d->timeLine->setDirection(checkState() == Qt::Unchecked ? QTimeLine::Forward : QTimeLine::Backward);
        d->timeLine->start();
    }
    QAbstractButton::nextCheckState();
    QCheckBox::checkStateSet();
}
