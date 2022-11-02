#include "qtblurbehindeffect.h"
#ifndef NO_OPENGLBLUR
#include "glblurfunctions.h"
#endif
#include "blur.h"

#include <QPainter>
#include <QWidget>
#include <QThread>
#include <QDebug>

class QtBlurBehindEffectPrivate
{
public:
#ifndef NO_OPENGLBLUR
    GLBlurFunctions glBlur;
#endif
    qint64 cacheKey;
    QImage sourceImage;
    QImage blurredImage;
    QRegion sourceRegion;
    QtBlurBehindEffect::BlurMethod blurringMethod;
    Qt::CoordinateSystem coordSystem;
    QBrush backgroundBrush;
    double sourceOpacity;
    double blurOpacity;
    double downsamplingFactor;
    int blurRadius;
    int maxThreadCount;
    bool sourceUpdated;

    QtBlurBehindEffectPrivate()
        : cacheKey(0)
        , blurringMethod(QtBlurBehindEffect::BlurMethod::StackBlur)
        , coordSystem(Qt::LogicalCoordinates)
        , sourceOpacity(1.0)
        , blurOpacity(1.0)
        , downsamplingFactor(2.0)
        , blurRadius(2)
        , maxThreadCount(1)
        , sourceUpdated(true)
    {
    }

    QImage blurImage(const QImage &_input)
    {
        switch(blurringMethod)
        {
        case QtBlurBehindEffect::BlurMethod::BoxBlur:
            return boxBlurImage(_input, blurRadius);
        case QtBlurBehindEffect::BlurMethod::StackBlur:
            return stackBlurImage(_input, blurRadius, maxThreadCount);
#ifndef NO_OPENGLBLUR
        case QtBlurBehindEffect::BlurMethod::GLBlur:
            return glBlur.blurImage_DualKawase(_input, 2, std::max(blurRadius - 2, 1));
#endif
        }
        return _input;
    }

    QPixmap grabSource(QWidget* _widget) const
    {
        if (!_widget)
            return QPixmap{};

        qreal dpr(1.0);
        if (const auto *paintDevice = _widget)
            dpr = paintDevice->devicePixelRatioF();
        else
            qWarning("QtBlurBehindEffect::grabSource: Painter not active");

        const bool isGlBlur = blurringMethod == QtBlurBehindEffect::BlurMethod::GLBlur;
        QPixmap image(_widget->size());
        image.setDevicePixelRatio(dpr);
        image.fill(isGlBlur ? _widget->palette().color(_widget->backgroundRole()) : Qt::transparent);
        _widget->render(&image, {}, {}, QWidget::DrawChildren);

        return image;
    }

    void renderImage(QPainter *_painter, const QImage &_image, const QBrush& _brush)
    {
        if (sourceRegion.isEmpty() || _image.isNull())
            return;

        const QRect bounds = sourceRegion.boundingRect();
        const QSize s = bounds.size();
        QRect r(QPoint(0, 0), s);

        QTransform transform;
        if (coordSystem == Qt::LogicalCoordinates)
            transform = _painter->worldTransform();

        _painter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
        if (coordSystem == Qt::LogicalCoordinates)
            _painter->translate(bounds.topLeft());

        if (!_brush.isOpaque() && !_brush.style() == Qt::NoBrush)
            _painter->fillRect(r, _brush);
        _painter->setOpacity(blurOpacity);
        _painter->drawImage(r, _image);

        if (coordSystem == Qt::LogicalCoordinates)
            _painter->setWorldTransform(transform);
    }
};

QtBlurBehindEffect::QtBlurBehindEffect(QWidget* parent)
    : QGraphicsEffect(parent)
    , d_ptr(new QtBlurBehindEffectPrivate)
{
}

QtBlurBehindEffect::~QtBlurBehindEffect() = default;

void QtBlurBehindEffect::setSourceOpacity(double opacity)
{
    Q_D(QtBlurBehindEffect);
    opacity = std::clamp(opacity, 0.0, 1.0);
    if (d->sourceOpacity == opacity)
        return;

    d->sourceOpacity = std::clamp(opacity, 0.0, 1.0);
    Q_EMIT sourceOpacityChanged(opacity);
    Q_EMIT repaintRequired();
    update();
}

double QtBlurBehindEffect::sourceOpacity() const
{
    Q_D(const QtBlurBehindEffect);
    return d->sourceOpacity;
}

void QtBlurBehindEffect::setBlurOpacity(double opacity)
{
    Q_D(QtBlurBehindEffect);
    opacity = std::clamp(opacity, 0.0, 1.0);
    if (d->blurOpacity == opacity)
        return;

    d->blurOpacity = opacity;
    Q_EMIT blurOpacityChanged(opacity);
    Q_EMIT repaintRequired();
    update();
}

double QtBlurBehindEffect::blurOpacity() const
{
    Q_D(const QtBlurBehindEffect);
    return d->blurOpacity;
}

void QtBlurBehindEffect::setBackgroundBrush(const QBrush& brush)
{
    Q_D(QtBlurBehindEffect);
    if (d->backgroundBrush == brush)
        return;

    d->backgroundBrush = brush;

    Q_EMIT backgroundBrushChanged(brush);
    Q_EMIT repaintRequired();
    update();
}

QBrush QtBlurBehindEffect::backgroundBrush() const
{
    Q_D(const QtBlurBehindEffect);
    return d->backgroundBrush;
}

void QtBlurBehindEffect::setBlurMethod(QtBlurBehindEffect::BlurMethod blurMethod)
{
    Q_D(QtBlurBehindEffect);
    if (d->blurringMethod == blurMethod)
        return;

    d->blurringMethod = blurMethod;
    Q_EMIT repaintRequired();
    update();
}

QtBlurBehindEffect::BlurMethod QtBlurBehindEffect::blurMethod() const
{
    Q_D(const QtBlurBehindEffect);
    return d->blurringMethod;
}

void QtBlurBehindEffect::setRegion(const QRegion& sourceRegion)
{
    Q_D(QtBlurBehindEffect);
    d->sourceRegion = sourceRegion;
    updateBoundingRect();
}

const QRegion &QtBlurBehindEffect::region() const
{
    Q_D(const QtBlurBehindEffect);
    return d->sourceRegion;
}

void QtBlurBehindEffect::setBlurRadius(int radius)
{
    Q_D(QtBlurBehindEffect);
    if (d->blurRadius == radius)
        return;

    d->blurRadius = radius;
    Q_EMIT blurRadiusChanged(radius);
    Q_EMIT repaintRequired();

    updateBoundingRect();
}

int QtBlurBehindEffect::blurRadius() const
{
    Q_D(const QtBlurBehindEffect);
    return d->blurRadius;
}

void QtBlurBehindEffect::setDownsampleFactor(double factor)
{
    Q_D(QtBlurBehindEffect);
    factor = std::max(factor, 1.0);

    if (d->downsamplingFactor == factor)
        return;

    d->downsamplingFactor = factor;
    Q_EMIT downsampleFactorChanged(factor);
    Q_EMIT repaintRequired();

    update();
}

double QtBlurBehindEffect::downsampleFactor() const
{
    Q_D(const QtBlurBehindEffect);
    return d->downsamplingFactor;
}

void QtBlurBehindEffect::setMaxThreadCount(int nthreads)
{
    Q_D(QtBlurBehindEffect);
    d->maxThreadCount = std::clamp(nthreads, 1, std::max(QThread::idealThreadCount(), 1));
}

int QtBlurBehindEffect::maxThreadCount() const
{
    Q_D(const QtBlurBehindEffect);
    return d->maxThreadCount;
}

void QtBlurBehindEffect::setCoordinateSystem(Qt::CoordinateSystem coordSystem)
{
    Q_D(QtBlurBehindEffect);
    if (d->coordSystem == coordSystem)
        return;

    d->coordSystem = coordSystem;
    Q_EMIT repaintRequired();
    update();
}

Qt::CoordinateSystem QtBlurBehindEffect::coordinateSystem() const
{
    Q_D(const QtBlurBehindEffect);
    return d->coordSystem;
}

void QtBlurBehindEffect::draw(QPainter* painter)
{
    Q_D(QtBlurBehindEffect);
    QWidget* w = qobject_cast<QWidget*>(parent());
    if (!w)
        return;

    const QRect bounds = d->sourceRegion.boundingRect();

    // grap widget source pixmap
    QPixmap pixmap = d->grabSource(w);
    // render source
    painter->drawPixmap(0, 0, pixmap);

    if (d->sourceOpacity > 0.0 && d->sourceOpacity < 1.0)
    {
        if (!d->sourceRegion.isEmpty() && bounds != w->rect())
            painter->setClipRegion(QRegion(w->rect()) -= d->sourceRegion);
        else if (!d->sourceRegion.isEmpty())
            painter->setClipRegion(d->sourceRegion);

        const double opacity = painter->opacity();
        painter->setOpacity(d->sourceOpacity);
        painter->drawPixmap(0, 0, pixmap);
        painter->setOpacity(opacity);
    }

    // get image blur region and downsample it
    if (!d->sourceRegion.isEmpty() && d->blurRadius > 1)
    {
        const double dpr = pixmap.devicePixelRatioF();
        const QSize s = (QSizeF(bounds.size()) * (dpr / d->downsamplingFactor)).toSize();
        QImage pixmapPart = std::move(pixmap.copy(bounds).scaled(s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage());
        if (d->sourceImage != pixmapPart)
        {
            d->sourceImage = pixmapPart;
            d->cacheKey = pixmapPart.cacheKey();
            d->sourceUpdated = true;
        }
    }
}

void QtBlurBehindEffect::render(QPainter* painter)
{
    Q_D(QtBlurBehindEffect);
    if (blurRadius() <= 1 || d->sourceImage.isNull())
        return;

    if (!d->sourceUpdated)
        return d->renderImage(painter, d->blurredImage, d->backgroundBrush);

    d->blurredImage = std::move(d->blurImage(d->sourceImage));
    d->renderImage(painter, d->blurredImage, d->backgroundBrush);
    d->sourceUpdated = false;
}

void QtBlurBehindEffect::render(QPainter* painter, const QPainterPath& clipPath)
{
    Q_D(QtBlurBehindEffect);
    if (blurRadius() <= 1 || d->sourceImage.isNull())
        return;

    if (clipPath.isEmpty())
        return render(painter);

    const QRect regionRect = d->sourceRegion.boundingRect();
    const QRectF targetBounds = clipPath.boundingRect();

    if (!regionRect.contains(targetBounds.toRect()))
    {
        qWarning() << "QtBlurBehindEffect::render: target path is outside of source region";
        return;
    }

    if (d->sourceUpdated)
    {
        d->blurredImage = std::move(d->blurImage(d->sourceImage));
        d->sourceUpdated = false;
    }

    QImage image = d->blurredImage.scaled(regionRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter->setOpacity(d->blurOpacity);
    painter->drawImage(QPointF{}, image, targetBounds);
}

