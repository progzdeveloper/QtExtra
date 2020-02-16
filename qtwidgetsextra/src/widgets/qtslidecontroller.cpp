#include "qtslidecontroller.h"


#include <QtCore/QScopedPointer>
#include <QtCore/QTimeLine>
#include <QtCore/QEvent>

#include <QtGui/QResizeEvent>

#include <QtGui/QPixmap>
#include <QtGui/QImage>
#include <QtGui/QPainter>

#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWizard>

#include <QtWidgets/QApplication>

#include <QMutex>


/*
class QtSlideControllerPrivate
{
public:
    QtSlideController* q_ptr;

    double opacity;
    int previousIndex;
    int slideDir;
    bool inversed;
    bool faded;

    QtSlideController::SlideEffects effectFlags;

    QStackedWidget* widget;
    QTimeLine *timeLine;
    QPixmap frames[2];
    QPixmap backbuffer;

    QtSlideControllerPrivate(QtSlideController* q);

    QPointF slidePoint(const QRectF& rect, qreal value, int frame) const;
    int currentDirection() const;
    void renderFrame(int frame, qreal value, QPainter& painter) const;

    void zoomFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const;
    void rollFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const;
    void rotateFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const;
    void shearFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const;
};


QtSlideControllerPrivate::QtSlideControllerPrivate(QtSlideController* q) :
    q_ptr(q),
    opacity(0.0),
    previousIndex(0),
    slideDir(QtSlideController::SlideUp),
    inversed(false),
    faded(true),
    widget(Q_NULLPTR)
{
    timeLine = new QTimeLine(900, q_ptr);
    timeLine->setUpdateInterval(5);
}

#if 0
QPointF QSlideControllerPrivate::slidePoint(const QRectF & rect, qreal value, int frame) const
{
    QPointF pt;
    //if (effectFlags.testFlag(QSlideController::ZoomEffect)) {
    //    return (-rect.center());
    //}

    if (effectFlags.testFlag(QSlideController::RollEffect)) {
        qreal sign = (frame ? -1 : 1);
        switch (currentDirection()) {
        case QSlideController::SlideUp:
            pt.setY(sign * rect.height() * value);
            break;
        case QSlideController::SlideDown:
            pt.setY(-sign * rect.height() * value);
            break;
        case QSlideController::SlideLeft:
            pt.setX(sign * rect.width() * value);
            break;
        case QSlideController::SlideRight:
            pt.setX(-sign * rect.width() * value);
            break;
        case QSlideController::NoSliding:
            break;
        }
    }
    return pt;
}
#endif

void QtSlideControllerPrivate::zoomFrame(QTransform& transform, const QRectF &, qreal value, int) const
{
    transform.scale(1.0 - value, 1.0 - value);
}

void QtSlideControllerPrivate::rollFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const
{
    int sign = (frame == 0 ? -1 : 1);
    switch (currentDirection()) {
    case QtSlideController::SlideUp:
        transform.translate(0, sign * rect.height() * value);
        break;
    case QtSlideController::SlideDown:
        transform.translate(0, -sign * rect.height() * value);
        break;
    case QtSlideController::SlideLeft:
        transform.translate(sign * rect.width() * value, 0);
        break;
    case QtSlideController::SlideRight:
        transform.translate(-sign * rect.width() * value, 0);
        break;
    case QtSlideController::NoSliding:
        break;
    }
}

void QtSlideControllerPrivate::rotateFrame(QTransform& transform, const QRectF &, qreal value, int frame) const
{
    if (frame == 1 && inversed) {
        transform.rotate(value * 90, Qt::YAxis);
        transform.scale(1.0 - value, 1.0);
    }
    if (frame == 0 && !inversed) {
        transform.rotate(value * 90, Qt::YAxis);
        transform.scale(1.0 - value, 1.0);
    }
}

void QtSlideControllerPrivate::shearFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const
{
    if (frame == 0 && inversed) {
        transform.translate(rect.width() * (value), 0);
        transform.scale(1.0 - value, 1.0);
    }
    if (frame == 1 && !inversed) {
        transform.translate(rect.width() * (value), 0);
        transform.scale(1.0 - value, 1.0);
    }
}





int QtSlideControllerPrivate::currentDirection() const
{
    if (!inversed)
        return slideDir;

    if ((slideDir == QtSlideController::SlideUp ||
         slideDir == QtSlideController::SlideDown))
        return (slideDir == QtSlideController::SlideUp ?
                    QtSlideController::SlideDown :
                    QtSlideController::SlideUp);

    if ((slideDir == QtSlideController::SlideLeft ||
         slideDir == QtSlideController::SlideRight))
        return (slideDir == QtSlideController::SlideLeft ?
                    QtSlideController::SlideRight :
                    QtSlideController::SlideLeft);
    return slideDir;
}


void QtSlideControllerPrivate::renderFrame(int frame, qreal value, QPainter & painter) const
{
    QRect r = backbuffer.rect();
    //QPointF c = r.center();
    QTransform transform = painter.transform();

    if (frame == 0)
        value = 1.0 - value;

    if (faded)
        painter.setOpacity(1.0 - value);

    for (int i = QtSlideController::ZoomEffect; i < QtSlideController::RollEffect + 1; i <<= 1)
    {
        if (!effectFlags.testFlag(static_cast<QtSlideController::SlideEffect>(i)))
            continue;

        switch (i)
        {
        case QtSlideController::ZoomEffect:
            zoomFrame(transform, r, value, frame);
            break;
        case QtSlideController::RotateEffect:
            rotateFrame(transform, r, value, frame);
            break;
        case QtSlideController::ShearEffect:
            shearFrame(transform, r, value, frame);
        case QtSlideController::RollEffect:
            rollFrame(transform, r, value, frame);
            break;
        }
    }

    painter.setTransform(transform, true);
    painter.drawPixmap(0, 0, frames[frame]);
    painter.resetTransform();
}

#if 0
void QSlideControllerPrivate::renderFrame(int frame, qreal value, QPainter & painter) const
{
    QRect r = backbuffer.rect();
    QPointF c = r.center();
    QTransform transform = painter.transform();

    if (frame == 1)
        value = 1.0 - value;

    QPointF p = slidePoint(backbuffer.rect(), 1.0 - value, frame);

    int sign = inversed ? 1 : -1;
    for (int i = QSlideController::FadeEffect; i < QSlideController::RollEffect + 1; i <<= 1)
    {
        if (!effectFlags.testFlag(static_cast<QSlideController::SlideEffect>(i)))
            continue;

        switch (i)
        {
        case QSlideController::FadeEffect:
            if (effectFlags.testFlag(QSlideController::RotateEffect))
                painter.setOpacity(1.0 - value);
            else
                painter.setOpacity(value);
            break;
        case QSlideController::ZoomEffect:
            transform.translate(c.x(), c.y());
            transform.scale(value, value);
            break;
        case QSlideController::RotateEffect:
            transform.rotate(sign * value * 90.0, Qt::YAxis);
            transform.scale(1.0 - value, 1.0);
            frame = frame == 0 ? 1 : 0;
            break;
        case QSlideController::ShearEffect:
            if (inversed) {
                transform.translate(r.width() * (frame == 0 ? (value) : 0), 0);
                transform.scale(1.0 - value, 1.0);
                frame = frame == 0 ? 1 : 0;
            } else {
                transform.translate(r.width() * (frame == 0 ? (1.0 - value) : 0), 0);
                transform.scale(value, 1.0);
            }
            break;
        }
    }
    painter.setTransform(transform, true);
    painter.drawPixmap(p, frames[frame]);
    painter.resetTransform();
}
#endif







QtSlideController::QtSlideController(QObject* parent) : 
    QObject(parent), d_ptr(new QtSlideControllerPrivate(this))
{
    Q_D(QtSlideController);
    connect(d->timeLine, &QTimeLine::valueChanged,
            this, &QtSlideController::render);

    connect(d->timeLine, &QTimeLine::finished,
            this, &QtSlideController::onAnimationFinished);
}

QtSlideController::~QtSlideController()
{
    delete d_ptr;
}

void QtSlideController::setWidget(QStackedWidget * w)
{
    Q_D(QtSlideController);
    if (d->widget) {
        disconnect(d->widget);
        d->widget->removeEventFilter(this);
    }
    if (w) {
        d->widget = w;
        connect(d->widget, SIGNAL(currentChanged(int)), SLOT(onCurrentChange(int)));
        d->widget->installEventFilter(this);
    }
}

QWidget * QtSlideController::widget() const
{
    Q_D(const QtSlideController);
    return d->widget;
}

void QtSlideController::setEffects(QtSlideController::SlideEffects flags)
{
    Q_D(QtSlideController);
    d->effectFlags = flags;
}

QtSlideController::SlideEffects QtSlideController::effects() const
{
    Q_D(const QtSlideController);
    return d->effectFlags;
}

void QtSlideController::setSlideDirection(int dir)
{
    Q_D(QtSlideController);
    d->slideDir = dir;
}

int QtSlideController::slideDirection() const
{
    Q_D(const QtSlideController);
    return d->slideDir;
}

void QtSlideController::setDuration(int ms)
{
    Q_D(QtSlideController);
    d->timeLine->setDuration(ms);
}

int QtSlideController::duration() const
{
    Q_D(const QtSlideController);
    return d->timeLine->duration();
}

void QtSlideController::setFaded(bool on)
{
    Q_D(QtSlideController);
    d->faded = on;
}

bool QtSlideController::isFaded() const
{
    Q_D(const QtSlideController);
    return d->faded;
}

QWidget * QtSlideController::currentWidget() const
{
    Q_D(const QtSlideController);
    return (d->widget ? d->widget->currentWidget() : Q_NULLPTR);
}

QWidget * QtSlideController::widget(int id) const
{
    Q_D(const QtSlideController);
    return (d->widget ? d->widget->widget(id) : Q_NULLPTR);
}

int QtSlideController::currentIndex() const
{
    Q_D(const QtSlideController);
    return (d->widget ? d->widget->currentIndex() : -1);
}

bool QtSlideController::eventFilter(QObject * obj, QEvent * e)
{
    Q_D(QtSlideController);
    if (obj == d->widget) {
        if (e->type() == QEvent::Paint) {
            if (d->timeLine->state() == QTimeLine::NotRunning) {
                //d->previousIndex = currentIndex();
                return QObject::eventFilter(obj, e);
            }
            else {
                QPainter painter(d->widget);
                //painter.drawImage(d->widget->rect(), d->backbuffer);
                painter.drawPixmap(d->widget->rect(), d->backbuffer);
                return true; // do NOT forward the event
            }
        }
        if (e->type() == QEvent::Resize) {
            QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(e);
            QRect r = currentWidget()->rect();
            d->frames[0] = currentWidget()->grab(r);
            d->frames[1] = widget(d->previousIndex)->grab(r);
            d->backbuffer = QPixmap(resizeEvent->size());
            qApp->processEvents();
        }
    }
    return QObject::eventFilter(obj, e);
}


void QtSlideController::onCurrentChange(int i)
{
    Q_D(QtSlideController);

    if (d->effectFlags == NoEffect)
        return;

    if (i == -1 || d->widget->count() == 1)
        return;

    QWidget* w = currentWidget();
    w->hide();

    QRect r = w->rect();
    d->frames[0] = widget(i)->grab(r);
    d->frames[1] = widget(d->previousIndex)->grab(r);

    d->inversed = (i - d->previousIndex > 0);

    if (d->timeLine->state() == QTimeLine::Running) {
        d->timeLine->stop();
    }

    d->backbuffer = QPixmap(d->widget->size());
    render(0);
    d->timeLine->start();
    d->previousIndex = currentIndex();
}

void QtSlideController::onAnimationFinished()
{
    Q_D(QtSlideController);
    QWidget* w = currentWidget();
    d->opacity = 0.0;
    w->show();
    d->widget->repaint();
}

void QtSlideController::render(qreal value)
{
    Q_D(QtSlideController);
    d->backbuffer.fill(Qt::transparent);

    QPainter painter(&d->backbuffer);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

    if (d->inversed) {
        d->renderFrame(1, value, painter);
        d->renderFrame(0, value, painter);
    }
    else {
        d->renderFrame(0, value, painter);
        d->renderFrame(1, value, painter);
    }

    d->widget->repaint();
}
*/


class QtSlideControllerPrivate
{
public:
    QtSlideController* q_ptr;

    double opacity;
    int previousIndex;
    int slideDir;
    bool inversed;
    bool faded;

    QtSlideController::SlideEffects effectFlags;

    QWidget* widget;
    QTimeLine *timeLine;
    QPixmap frames[2];
    QPixmap backbuffer;

    QtSlideControllerPrivate(QtSlideController* q);

    QPointF slidePoint(const QRectF& rect, qreal value, int frame) const;
    int currentDirection() const;
    void renderFrame(int frame, qreal value, QPainter& painter) const;

    void zoomFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const;
    void rollFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const;
    void rotateFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const;
    void shearFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const;
};


QtSlideControllerPrivate::QtSlideControllerPrivate(QtSlideController* q) :
    q_ptr(q),
    opacity(0.0),
    previousIndex(0),
    slideDir(QtSlideController::SlideUp),
    inversed(false),
    faded(true),
    widget(Q_NULLPTR)
{
    timeLine = new QTimeLine(900, q_ptr);
    timeLine->setUpdateInterval(5);
}

void QtSlideControllerPrivate::zoomFrame(QTransform& transform, const QRectF &, qreal value, int) const
{
    transform.scale(1.0 - value, 1.0 - value);
}

void QtSlideControllerPrivate::rollFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const
{
    int sign = (frame == 0 ? -1 : 1);
    switch (currentDirection()) {
    case QtSlideController::SlideUp:
        transform.translate(0, sign * rect.height() * value);
        break;
    case QtSlideController::SlideDown:
        transform.translate(0, -sign * rect.height() * value);
        break;
    case QtSlideController::SlideLeft:
        transform.translate(sign * rect.width() * value, 0);
        break;
    case QtSlideController::SlideRight:
        transform.translate(-sign * rect.width() * value, 0);
        break;
    case QtSlideController::NoSliding:
        break;
    }
}

void QtSlideControllerPrivate::rotateFrame(QTransform& transform, const QRectF &, qreal value, int frame) const
{
    if (frame == 1 && inversed) {
        transform.rotate(value * 90, Qt::YAxis);
        transform.scale(1.0 - value, 1.0);
    }
    if (frame == 0 && !inversed) {
        transform.rotate(value * 90, Qt::YAxis);
        transform.scale(1.0 - value, 1.0);
    }
}

void QtSlideControllerPrivate::shearFrame(QTransform& transform, const QRectF & rect, qreal value, int frame) const
{
    if (frame == 0 && inversed) {
        transform.translate(rect.width() * (value), 0);
        transform.scale(1.0 - value, 1.0);
    }
    if (frame == 1 && !inversed) {
        transform.translate(rect.width() * (value), 0);
        transform.scale(1.0 - value, 1.0);
    }
}





int QtSlideControllerPrivate::currentDirection() const
{
    if (!inversed)
        return slideDir;

    if ((slideDir == QtSlideController::SlideUp ||
         slideDir == QtSlideController::SlideDown))
        return (slideDir == QtSlideController::SlideUp ?
                    QtSlideController::SlideDown :
                    QtSlideController::SlideUp);

    if ((slideDir == QtSlideController::SlideLeft ||
         slideDir == QtSlideController::SlideRight))
        return (slideDir == QtSlideController::SlideLeft ?
                    QtSlideController::SlideRight :
                    QtSlideController::SlideLeft);
    return slideDir;
}


void QtSlideControllerPrivate::renderFrame(int frame, qreal value, QPainter & painter) const
{
    QRect r = backbuffer.rect();
    //QPointF c = r.center();
    QTransform transform = painter.transform();

    if (frame == 0)
        value = 1.0 - value;

    if (faded)
        painter.setOpacity(1.0 - value);

    for (int i = QtSlideController::ZoomEffect; i < QtSlideController::RollEffect + 1; i <<= 1)
    {
        if (!effectFlags.testFlag(static_cast<QtSlideController::SlideEffect>(i)))
            continue;

        switch (i)
        {
        case QtSlideController::ZoomEffect:
            zoomFrame(transform, r, value, frame);
            break;
        case QtSlideController::RotateEffect:
            rotateFrame(transform, r, value, frame);
            break;
        case QtSlideController::ShearEffect:
            shearFrame(transform, r, value, frame);
        case QtSlideController::RollEffect:
            rollFrame(transform, r, value, frame);
            break;
        }
    }

    painter.setTransform(transform, true);
    painter.drawPixmap(0, 0, frames[frame]);
    painter.resetTransform();
}






QtSlideController::QtSlideController(QObject* parent) :
    QObject(parent), d_ptr(new QtSlideControllerPrivate(this))
{
    Q_D(QtSlideController);
    connect(d->timeLine, &QTimeLine::valueChanged,
            this, &QtSlideController::render);

    connect(d->timeLine, &QTimeLine::finished,
            this, &QtSlideController::onAnimationFinished);
}

QtSlideController::~QtSlideController()
{
}

void QtSlideController::setWidget(QStackedWidget *w)
{
    Q_D(QtSlideController);
    if (d->widget) {
        disconnect(d->widget);
        d->widget->removeEventFilter(this);
    }
    if (w) {
        d->widget = w;
        connect(d->widget, SIGNAL(currentChanged(int)), SLOT(onCurrentChange(int)));
        d->widget->installEventFilter(this);
    }
}

void QtSlideController::setWidget(QWizard * w)
{
    Q_D(QtSlideController);
    if (d->widget) {
        disconnect(d->widget);
        d->widget->removeEventFilter(this);
    }
    if (w) {
        d->widget = w;
        connect(d->widget, SIGNAL(currentIdChanged(int)), SLOT(onCurrentChange(int)));
        d->widget->installEventFilter(this);
    }
}

QWidget * QtSlideController::widget() const
{
    Q_D(const QtSlideController);
    return d->widget;
}

void QtSlideController::setEffects(QtSlideController::SlideEffects flags)
{
    Q_D(QtSlideController);
    d->effectFlags = flags;
}

QtSlideController::SlideEffects QtSlideController::effects() const
{
    Q_D(const QtSlideController);
    return d->effectFlags;
}

void QtSlideController::setSlideDirection(int dir)
{
    Q_D(QtSlideController);
    d->slideDir = dir;
}

int QtSlideController::slideDirection() const
{
    Q_D(const QtSlideController);
    return d->slideDir;
}

void QtSlideController::setDuration(int ms)
{
    Q_D(QtSlideController);
    d->timeLine->setDuration(ms);
}

int QtSlideController::duration() const
{
    Q_D(const QtSlideController);
    return d->timeLine->duration();
}

void QtSlideController::setFaded(bool on)
{
    Q_D(QtSlideController);
    d->faded = on;
}

bool QtSlideController::isFaded() const
{
    Q_D(const QtSlideController);
    return d->faded;
}

QWidget * QtSlideController::currentWidget() const
{
    Q_D(const QtSlideController);
    if (qobject_cast<QWizard*>(d->widget) != Q_NULLPTR)
        return (static_cast<QWizard*>(d->widget) ? static_cast<QWizard*>(d->widget)->currentPage() : Q_NULLPTR);
    else
        return (static_cast<QStackedWidget*>(d->widget) ? static_cast<QStackedWidget*>(d->widget)->currentWidget() : Q_NULLPTR);
}

QWidget * QtSlideController::widget(int id) const
{
    Q_D(const QtSlideController);
    if (qobject_cast<QWizard*>(d->widget) != Q_NULLPTR)
        return (static_cast<QWizard*>(d->widget) ? static_cast<QWizard*>(d->widget)->page(id) : Q_NULLPTR);
    else
        return (static_cast<QStackedWidget*>(d->widget) ? static_cast<QStackedWidget*>(d->widget)->widget(id) : Q_NULLPTR);
}

int QtSlideController::currentIndex() const
{
    Q_D(const QtSlideController);
    if (qobject_cast<QWizard*>(d->widget) != Q_NULLPTR)
        return (static_cast<QWizard*>(d->widget) ? static_cast<QWizard*>(d->widget)->currentId() : -1);
    else
        return (static_cast<QStackedWidget*>(d->widget) ? static_cast<QStackedWidget*>(d->widget)->currentIndex() : -1);
}

int QtSlideController::count() const
{
    Q_D(const QtSlideController);
    if (qobject_cast<QWizard*>(d->widget) != Q_NULLPTR)
        return (static_cast<QWizard*>(d->widget)->pageIds().size());
    else
        return (static_cast<QStackedWidget*>(d->widget)->count());
}

bool QtSlideController::eventFilter(QObject * obj, QEvent * e)
{
    Q_D(QtSlideController);

    static QMutex mtx;
    if (obj == d->widget) {
        if (e->type() == QEvent::Paint) {
            if (d->timeLine->state() == QTimeLine::NotRunning) {
                //d->previousIndex = currentIndex();
                return QObject::eventFilter(obj, e);
            }
            else {
                QPainter painter(d->widget);
                //painter.drawPixmap(d->widget->rect(), d->backbuffer);
                QWidget* w = currentWidget();
                QPoint p = w->mapTo(d->widget, w->pos());
                p.rx() -= (w->layout() == Q_NULLPTR ? 0 : w->layout()->spacing());
                //p.ry() -= (w->layout() == Q_NULLPTR ? 0 : w->layout()->contentsMargins().top()/2);
                painter.drawPixmap(p, d->backbuffer);
                return true; // do NOT forward the event
            }
        }
        if (e->type() == QEvent::Resize) {

            QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(e);

            if (mtx.tryLock()) {
                QWidget* current = currentWidget();
                QWidget* previous = widget(d->previousIndex);
                QRect r = current->rect();
                d->frames[0] = current->grab(r);
                d->frames[1] = previous->grab(r);
                mtx.unlock();
            }

            d->backbuffer = QPixmap(resizeEvent->size());
        }
        if (e->type() == QEvent::Show) {
            d->timeLine->setCurrentTime(d->timeLine->duration());
            d->timeLine->stop();
        }
    }
    return QObject::eventFilter(obj, e);
}


void QtSlideController::onCurrentChange(int i)
{
    Q_D(QtSlideController);

    if (d->effectFlags == NoEffect)
        return;

    if (i == -1)
        return;

    if (count() == 1)
        return;

    QWidget* w = currentWidget();
    w->hide();

    QRect r = w->rect();
    d->frames[0] = widget(i)->grab(r);
    d->frames[1] = widget(d->previousIndex)->grab(r);

    d->inversed = (i - d->previousIndex > 0);

    if (d->timeLine->state() == QTimeLine::Running) {
        d->timeLine->stop();
    }

    d->backbuffer = QPixmap(d->widget->size());
    render(0);
    d->timeLine->start();
    d->previousIndex = currentIndex();
}

void QtSlideController::onAnimationFinished()
{
    Q_D(QtSlideController);
    QWidget* w = currentWidget();
    d->opacity = 0.0;
    w->show();
    d->widget->repaint();
}

void QtSlideController::render(qreal value)
{
    Q_D(QtSlideController);
    d->backbuffer.fill(Qt::transparent);

    QPainter painter(&d->backbuffer);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

    if (d->inversed) {
        d->renderFrame(1, value, painter);
        d->renderFrame(0, value, painter);
    }
    else {
        d->renderFrame(0, value, painter);
        d->renderFrame(1, value, painter);
    }

    d->widget->repaint();
}




