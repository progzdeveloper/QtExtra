#include "qtoverviewwidget.h"
#include <QPointer>
#include <QAbstractScrollArea>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QEvent>
#include <QMouseEvent>

#include <QPixmap>
#include <QStylePainter>

#include <QElapsedTimer>
#include <QBasicTimer>
#include <QApplication>
#include <QPaintEngine>
#include <QTimer>

#include <QScopedValueRollback>

static QPoint invalidPoint(-1, -1);

class QtOverviewWidgetPrivate
{
public:
    QtOverviewWidget* q_ptr;
    QPointer<QAbstractScrollArea> area;
    QTimer* timer;
    QPixmap pixmap;
    QRect pixmapRect;
    QRect contentRect;
    QPoint pos;
    bool updatable;
    bool updating;

    void createTimer();
    QPointF scaleFactor() const;
    void grabViewport();
    void updateContentRect();

    QtOverviewWidgetPrivate(QtOverviewWidget* q) :
        q_ptr(q), pos(invalidPoint),
        updatable(false), updating(false)
    {}
};

void QtOverviewWidgetPrivate::createTimer()
{
    timer = new QTimer(q_ptr);
    timer->setInterval(100);
    QObject::connect(timer, &QTimer::timeout, q_ptr, &QtOverviewWidget::updatePixmap);
}

QPointF QtOverviewWidgetPrivate::scaleFactor() const
{
    if (!area)
        return QPointF(1.0, 1.0);

    const QScrollBar* hbar = area->horizontalScrollBar();
    const QScrollBar* vbar = area->verticalScrollBar();
    const int width = hbar->maximum() + hbar->pageStep();
    const int height = vbar->maximum() + vbar->pageStep();

    return QPointF(width / (double)pixmapRect.width(), height / (double)pixmapRect.height());
}

void QtOverviewWidgetPrivate::grabViewport()
{
    if (updating)
        return;

    if (!area)
        return;

    QWidget* viewport = area->viewport();
    if (!viewport)
        return;

    QScrollBar* hbar = area->horizontalScrollBar();
    QScrollBar* vbar = area->verticalScrollBar();

    const QPoint oldPos(hbar->value(), vbar->value());

    hbar->setValue(0);
    vbar->setValue(0);

    const int width = hbar->maximum() + hbar->pageStep();
    const int height = vbar->maximum() + vbar->pageStep();

    {
        QScopedValueRollback<bool> lock(updating, true);
        pixmap = viewport->grab(QRect(0, 0, width, height)).scaled(q_ptr->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    hbar->setValue(oldPos.x());
    vbar->setValue(oldPos.y());

    pixmapRect = pixmap.rect();
    pixmapRect.moveCenter(q_ptr->rect().center());

    updateContentRect();
}

void QtOverviewWidgetPrivate::updateContentRect()
{
    if (!area)
    {
        contentRect = QRect();
        return;
    }

    QWidget* viewport = area->viewport();
    if (!viewport)
    {
        contentRect = QRect();
        return;
    }

    const QRectF r = viewport->rect();

    const QPointF scale = scaleFactor();
    const double x = area->horizontalScrollBar()->value();
    const double y = area->verticalScrollBar()->value();
    const double w = r.width();
    const double h = r.height();

    contentRect.setRect(pixmapRect.x(), pixmapRect.y(), w / scale.x(), h / scale.y());
    contentRect.translate(x / scale.x(), y / scale.y());
}


QtOverviewWidget::QtOverviewWidget(QWidget *parent, Qt::WindowFlags flags) :
    QWidget(parent, flags), d_ptr(new QtOverviewWidgetPrivate(this))
{
    setMouseTracking(true);
    setCursor(Qt::ArrowCursor);

    Q_D(QtOverviewWidget);
    d->createTimer();
}

QtOverviewWidget::~QtOverviewWidget()
{
}

void QtOverviewWidget::setArea(QAbstractScrollArea *area)
{
    Q_D(QtOverviewWidget);
    if (d->area)
    {
        d->area->removeEventFilter(this);
        disconnect(d->area->verticalScrollBar(), &QScrollBar::valueChanged, this, &QtOverviewWidget::updateContentRect);
        disconnect(d->area->verticalScrollBar(), &QScrollBar::rangeChanged, this, &QtOverviewWidget::refresh);

        disconnect(d->area->horizontalScrollBar(), &QScrollBar::valueChanged, this, &QtOverviewWidget::updateContentRect);
        disconnect(d->area->horizontalScrollBar(), &QScrollBar::rangeChanged, this, &QtOverviewWidget::refresh);

        disconnect(d->area, &QObject::destroyed, this, &QWidget::close);
    }

    d->area = area;

    if (d->area)
    {
        d->area->installEventFilter(this);
        connect(d->area->verticalScrollBar(), &QScrollBar::valueChanged, this, &QtOverviewWidget::updateContentRect);
        connect(d->area->verticalScrollBar(), &QScrollBar::rangeChanged, this, &QtOverviewWidget::refresh);

        connect(d->area->horizontalScrollBar(), &QScrollBar::valueChanged, this, &QtOverviewWidget::updateContentRect);
        connect(d->area->horizontalScrollBar(), &QScrollBar::rangeChanged, this, &QtOverviewWidget::refresh);

        connect(d->area, &QObject::destroyed, this, &QWidget::close);
    }
}

QAbstractScrollArea *QtOverviewWidget::area() const
{
    Q_D(const QtOverviewWidget);
    return d->area;
}

void QtOverviewWidget::setUpdatable(bool on)
{
    Q_D(QtOverviewWidget);
    d->updatable = on;
    if (isVisible() && d->area)
        d->timer->start();
}

bool QtOverviewWidget::isUpdatable()
{
    Q_D(const QtOverviewWidget);
    return d->updatable;
}

void QtOverviewWidget::setUpdateInterval(int msec)
{
    Q_D(QtOverviewWidget);
    if (d->timer->interval() == msec)
        return;

    d->timer->setInterval(msec);
    Q_EMIT updateIntervalChanged(msec);
}

int QtOverviewWidget::updateInterval() const
{
    Q_D(const QtOverviewWidget);
    return d->timer->interval();
}

void QtOverviewWidget::updatePixmap()
{
    Q_D(QtOverviewWidget);
    if (!isVisible())
        return;

    d->grabViewport();
    update();
}

void QtOverviewWidget::updateContentRect()
{
    Q_D(QtOverviewWidget);
    if (!isVisible())
        return;

    d->updateContentRect();
    update();
}

void QtOverviewWidget::refresh()
{
    Q_D(QtOverviewWidget);
    if (!isVisible())
        return;

    d->grabViewport();
    d->updateContentRect();
    update();
}


void QtOverviewWidget::drawContentRect(QPainter *painter, const QRect &r)
{
    QStyle* style = this->style();
    if (style == Q_NULLPTR)
        style = qApp->style();

    if (style == Q_NULLPTR) {
        painter->setPen(QPen(Qt::red, 1));
        painter->setBrush(QColor(128, 128, 128, 90));
        painter->drawRect(r.adjusted(1, 1, -1, -1));
    } else {
        QStyleOptionRubberBand option;
        option.initFrom(this);
        option.rect = r;
        painter->setOpacity(0.4);
        style->drawControl(QStyle::CE_RubberBand, &option, painter, this);
    }
}

bool QtOverviewWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(QtOverviewWidget);

    if (!isVisible())
        return QWidget::eventFilter(watched, event);

    if (watched != d->area)
        return QWidget::eventFilter(watched, event);

    QWidget* viewport = d->area->viewport();
    if (!viewport)
        return QWidget::eventFilter(watched, event);

    switch(event->type())
    {
    case QEvent::Hide:
        if (watched == viewport || watched == d->area)
            hide();
        break;
    case QEvent::Resize:
        if (watched == viewport || watched == d->area)
            refresh();
        break;
    case QEvent::EnabledChange:
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::PaletteChange:
        if (watched == viewport || watched == d->area)
            refresh();
        break;
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonRelease:
    case QEvent::KeyRelease:
        if (watched == d->area)
            QTimer::singleShot(0, this, [this]() { updatePixmap(); });
        break;
    default:
        break;
    }

    return QWidget::eventFilter(watched, event);
}

void QtOverviewWidget::showEvent(QShowEvent *event)
{
    Q_D(QtOverviewWidget);
    QWidget::showEvent(event);
    QTimer::singleShot(0, this, &QtOverviewWidget::refresh);
    if (d->updatable)
        d->timer->start();
}

void QtOverviewWidget::hideEvent(QHideEvent *event)
{
    Q_D(QtOverviewWidget);
    d->pixmap = QPixmap(); // release pixmap
    QWidget::hideEvent(event);
}
void QtOverviewWidget::paintEvent(QPaintEvent *event)
{
    Q_D(QtOverviewWidget);
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.drawPixmap(d->pixmapRect, d->pixmap);
    drawContentRect(&painter, d->contentRect);
}

void QtOverviewWidget::resizeEvent(QResizeEvent *event)
{
    Q_D(QtOverviewWidget);
    QWidget::resizeEvent(event);

    d->pixmapRect = d->pixmap.rect();
    d->pixmapRect.moveCenter(rect().center());
    d->updateContentRect();

    const QSize oldSize = event->oldSize();
    const QSize newSize = event->size();
    const QSize pixmapSize = d->pixmap.size();

    if (newSize.width() == oldSize.width())
    {
        updatePixmap();
    }
    else if (newSize.height() == oldSize.height())
    {
        const int widgetSide = std::min(newSize.width(), newSize.height());
        const int pixmapSide = std::max(pixmapSize.width(), pixmapSize.height());

        if (widgetSide != pixmapSide)
            updatePixmap();
        else
            update();
    }
    else
    {
        updatePixmap();
    }
}

void QtOverviewWidget::mousePressEvent(QMouseEvent *event)
{
    Q_D(QtOverviewWidget);
    switch (event->button())
    {
    case Qt::RightButton:
        hide();
        break;
    case Qt::LeftButton:
        if (d->contentRect.contains(event->pos()))
        {
            d->pos = event->pos();
            setCursor(Qt::ClosedHandCursor);
        }
        updatePixmap();
        break;
    default:
        break;
    }

    QWidget::mousePressEvent(event);
}

void QtOverviewWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(QtOverviewWidget);
    if (event->buttons() == Qt::NoButton && d->pos == invalidPoint)
    {
        if (d->contentRect.contains(event->pos())) {
            if (cursor().shape() != Qt::OpenHandCursor)
                setCursor(Qt::OpenHandCursor);
        } else {
            if (cursor().shape() != Qt::ArrowCursor)
                setCursor(Qt::ArrowCursor);
        }
    }

    if ((event->buttons() & Qt::LeftButton) && d->pos != invalidPoint)
    {
        const QPointF scale = d->scaleFactor();
        QScrollBar* hbar = d->area->horizontalScrollBar();
        QScrollBar* vbar = d->area->verticalScrollBar();

        QSignalBlocker blocker(d->area);
        const QPoint dist = (event->pos() - d->pos) * std::min(scale.x(), scale.y());
        hbar->setValue(hbar->value() + dist.x());
        vbar->setValue(vbar->value() + dist.y());
        d->pos = event->pos();
        d->updateContentRect();
    }

    QWidget::mouseMoveEvent(event);
}

void QtOverviewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(QtOverviewWidget);
    setCursor(d->contentRect.contains(event->pos()) ? Qt::OpenHandCursor : Qt::ArrowCursor);
    if (event->button() == Qt::LeftButton)
    {
        Q_D(QtOverviewWidget);
        d->pos = QPoint(-1, -1);
        update();
    }

    QWidget::mouseReleaseEvent(event);
}

