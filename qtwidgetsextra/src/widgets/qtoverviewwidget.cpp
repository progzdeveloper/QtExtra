#include "qtoverviewwidget.h"
#include <QPointer>
#include <QAbstractScrollArea>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QEvent>
#include <QMouseEvent>

#include <QPixmap>
#include <QPainter>

#include <QElapsedTimer>
#include <QBasicTimer>
#include <QApplication>
#include <QPaintEngine>



class QtOverviewWidgetPrivate
{
public:
    QtOverviewWidget* q_ptr;
    QPointer<QAbstractScrollArea> area;
    QPointer<QWidget> viewport;
    QBasicTimer timer;
    QPixmap pixmap;
    QRect cr;
    QSize size;
    QPoint pos;
    qint64 elapsed;
    double factor;

    void grabViewport(const QRegion &region = QRegion());
    QPoint origin(const QSize &s);
    void updateSize();
    void updateContentRect();

    QtOverviewWidgetPrivate(QtOverviewWidget* q) :
        q_ptr(q), pos(-1, -1), elapsed(1000), factor(4) {}
};

void QtOverviewWidgetPrivate::grabViewport(const QRegion& region)
{
    QPoint oldPos(area->horizontalScrollBar()->value(), area->verticalScrollBar()->value());

    int width = (area->horizontalScrollBar()->maximum() + area->horizontalScrollBar()->pageStep());
    area->horizontalScrollBar()->setValue(0);
    area->verticalScrollBar()->setValue(0);

    QSize s = size / factor;
    if (pixmap.size() != s) {
        pixmap = QPixmap(s);
        //pixmap.setDevicePixelRatio(1.0/factor);
    }

    QPainter painter(&pixmap);
    painter.scale(1/factor, 1/factor);
    if (region.isEmpty()) {
        if (viewport)
            viewport->render(&painter, QPoint(), QRect(0, 0, size.width(), size.height()));
    }
    else {

        QRegion reg;
        for (auto it = region.cbegin(); it != region.end(); ++it) {
            QRect r = *it;
            if (r.width() != width) {
                r.setX(0);
                r.setWidth(width);
            }
            reg += r;
        }
        QElapsedTimer elapsedTimer;
        elapsedTimer.start();
        if (viewport)
            viewport->render(&painter, QPoint(), reg);
        elapsed = qMax(elapsedTimer.elapsed(), elapsed);
        timer.start(elapsed + 10, q_ptr);
    }

    area->horizontalScrollBar()->setValue(oldPos.x());
    area->verticalScrollBar()->setValue(oldPos.y());
}

QPoint QtOverviewWidgetPrivate::origin(const QSize& s)
{
    QPoint p = area->rect().bottomRight();
    int x = p.x() - (s.width() + (area->verticalScrollBar()->isVisible() ? area->verticalScrollBar()->width(): 0));
    int y = p.y() - (s.height() + (area->horizontalScrollBar()->isVisible() ? area->horizontalScrollBar()->height() - 16 : 0));
    return QPoint(x,y);
}

void QtOverviewWidgetPrivate::updateSize()
{
    int width = (area->horizontalScrollBar()->maximum() + area->horizontalScrollBar()->pageStep());
    int height = (area->verticalScrollBar()->maximum() + area->verticalScrollBar()->pageStep());
    factor = qMax((width / (double)qMin(area->width(), 256)), (height / ((double)area->height())));
    if (factor == 0) {
        factor = 8;
    }
    size.setWidth(width);
    size.setHeight(height);
}

void QtOverviewWidgetPrivate::updateContentRect()
{
    QRectF r = viewport->rect();
    cr.setX(0);
    cr.setY(0);
    cr.setWidth(r.width() / factor);
    cr.setHeight(r.height() / factor);
    cr.translate(area->horizontalScrollBar()->value() /  factor,
                 area->verticalScrollBar()->value() /  factor);
}




QtOverviewWidget::QtOverviewWidget(QWidget *parent, Qt::WindowFlags flags) :
    QWidget(parent, Qt::Tool|flags), d_ptr(new QtOverviewWidgetPrivate(this))
{
    setMouseTracking(true);
    setCursor(Qt::ArrowCursor);
    //setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QtOverviewWidget::~QtOverviewWidget()
{
    Q_D(QtOverviewWidget);
    d->timer.stop();
}

void QtOverviewWidget::setArea(QAbstractScrollArea *area)
{
    Q_D(QtOverviewWidget);
    d->area = area;
    if (area) {
        d->viewport = area->viewport();
        if (d->viewport) {
            d->area->installEventFilter(this);
            d->viewport->installEventFilter(this);
            d->updateSize();
            //setFixedSize(d->size / d->factor);
            //repaint();

            connect(d->area->verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(refresh()));
            connect(d->area->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), SLOT(updatePixmap()));

            connect(d->area->horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(refresh()));
            connect(d->area->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), SLOT(updatePixmap()));

            connect(d->area, SIGNAL(destroyed(QObject*)), SLOT(close()));

            move(d->viewport->rect().topLeft());
        }
    }
}

QAbstractScrollArea *QtOverviewWidget::area() const
{
    Q_D(const QtOverviewWidget);
    return d->area;
}


bool QtOverviewWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(QtOverviewWidget);

    if (!isVisible())
        return QWidget::eventFilter(watched, event);

    if (watched != d->area && watched != d->viewport)
        return QWidget::eventFilter(watched, event);

    switch(event->type())
    {
    case QEvent::Hide:
        if (watched == d->viewport || watched == d->area) {
            this->hide();
        }
    case QEvent::Resize:
        if (watched == d->viewport) {
            d->updateSize();
            d->updateContentRect();
            //d->timer.start(16, this);
            /*if (d->cr.size().width() >= d->area->width() / 4 ||
                d->cr.size().height() >= d->area->height() / 4)
                hide();
            else {*/
                //d->updateContentRect();
                //setFixedSize(d->size / d->factor);
                //resize(d->size / d->factor);
                //d->elapsedTimer.start();
            //}
            //qApp->processEvents();
        }
        break;
    case QEvent::Paint:
        /*if (watched == d->viewport || watched == d->area) {
            d->timer.start(d->elapsed, this);
        }*/
        break;
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        if (watched == d->viewport || watched == d->area) {
            d->grabViewport(d->viewport->visibleRegion());
            update();
        }
        break;
    default:
        break;
    }
    return QWidget::eventFilter(watched, event);
}

void QtOverviewWidget::timerEvent(QTimerEvent *)
{
    Q_D(QtOverviewWidget);
    d->grabViewport();
    d->updateSize();
    d->updateContentRect();
    update();
    d->timer.stop();
    //QObject::timerEvent(event);
}

QSize QtOverviewWidget::maximumSize() const
{
    Q_D(const QtOverviewWidget);
    return d->size;
}

void QtOverviewWidget::showEvent(QShowEvent *event)
{
    Q_D(QtOverviewWidget);
    d->grabViewport();
    d->updateSize();
    d->updateContentRect();

    QWidget::showEvent(event);
    // shedule painting (approx. 25 fps)
    //d->timer.start(72, this);
}

void QtOverviewWidget::hideEvent(QHideEvent *event)
{
    Q_D(QtOverviewWidget);
    //d->pixmap = QPixmap(); // release pixmap
    d->timer.stop();
    QWidget::hideEvent(event);
}

void QtOverviewWidget::resizeEvent(QResizeEvent *event)
{
    Q_D(QtOverviewWidget);
    d->updateSize();
    d->grabViewport();
    d->updateContentRect();
    QWidget::resizeEvent(event);
    // shedule painting
    //d->timer.start(10, this);
}

void QtOverviewWidget::mousePressEvent(QMouseEvent *event)
{
    Q_D(QtOverviewWidget);
    switch (event->button()) {
    case Qt::RightButton:
        hide();
        break;
    case Qt::LeftButton:
        if (d->cr.contains(event->pos())) {
            d->pos = event->pos();
            setCursor(Qt::ClosedHandCursor);
        }
        break;
    default:
        break;
    }

    QWidget::mousePressEvent(event);
}

void QtOverviewWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(QtOverviewWidget);
    if (event->buttons() == Qt::NoButton && d->pos == QPoint(-1, -1)) {
        if (d->cr.contains(event->pos())) {
            if (cursor().shape() != Qt::OpenHandCursor)
                setCursor(Qt::OpenHandCursor);
        } else {
            if (cursor().shape() != Qt::ArrowCursor)
                setCursor(Qt::ArrowCursor);
        }
    }

    if ((event->buttons() & Qt::LeftButton) && d->pos != QPoint(-1, -1)) {
        d->area->blockSignals(true);
        QPoint dist = (event->pos() - d->pos) * d->factor;
        int x = d->area->horizontalScrollBar()->value();
        d->area->horizontalScrollBar()->setValue(x + dist.x());
        int y = d->area->verticalScrollBar()->value();
        d->area->verticalScrollBar()->setValue(y + dist.y());
        d->pos = event->pos();
        d->updateContentRect();
        d->area->blockSignals(false);
    }
    QWidget::mouseMoveEvent(event);
}

void QtOverviewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(QtOverviewWidget);
    setCursor(d->cr.contains(event->pos()) ? Qt::OpenHandCursor : Qt::ArrowCursor);
    if (event->button() == Qt::LeftButton) {
        Q_D(QtOverviewWidget);
        d->pos = QPoint(-1, -1);
        update();
    }

    QWidget::mouseReleaseEvent(event);
}


void QtOverviewWidget::paintEvent(QPaintEvent *event)
{
    Q_D(QtOverviewWidget);
    QPainter painter(this);
    painter.drawPixmap(rect(), d->pixmap);
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
    drawContentRect(&painter, d->cr);
    QWidget::paintEvent(event);
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
        option.rect = r.adjusted(1, 1, -1, 0);
        painter->setOpacity(0.4);
        style->drawControl(QStyle::CE_RubberBand, &option, painter, this);
    }
}

void QtOverviewWidget::updatePixmap()
{
    Q_D(QtOverviewWidget);
    //d->grabViewport();
    d->updateSize();
    setFixedSize(d->size / d->factor);
}

void QtOverviewWidget::refresh()
{
    Q_D(QtOverviewWidget);
    d->updateContentRect();
    update();
}



