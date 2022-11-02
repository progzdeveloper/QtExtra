#pragma once

#include <QtWidgetsExtra>
#include <QGraphicsEffect>

class QTWIDGETSEXTRA_EXPORT QtBlurBehindEffect :
        public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(BlurMethod blurMethod READ blurMethod WRITE setBlurMethod)
    Q_PROPERTY(int blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
    Q_PROPERTY(double blurOpacity READ blurOpacity WRITE setBlurOpacity NOTIFY blurOpacityChanged)
    Q_PROPERTY(double sourceOpacity READ sourceOpacity WRITE setSourceOpacity NOTIFY sourceOpacityChanged)
    Q_PROPERTY(double downsampleFactor READ downsampleFactor WRITE setDownsampleFactor NOTIFY downsampleFactorChanged)
    Q_PROPERTY(QBrush backgroundBrush READ backgroundBrush WRITE setBackgroundBrush NOTIFY backgroundBrushChanged)

public:
    enum class BlurMethod
    {
        BoxBlur = 0,
        StackBlur,
#ifndef NO_OPENGLBLUR
        GLBlur
#endif
    };
    Q_ENUM(BlurMethod)

    QtBlurBehindEffect(QWidget* parent = Q_NULLPTR);
    ~QtBlurBehindEffect();

    void render(QPainter* painter);
    void render(QPainter* painter, const QPainterPath& clipPath);

    void setSourceOpacity(double opacity);
    double sourceOpacity() const;

    void setBlurOpacity(double opacity);
    double blurOpacity() const;

    void setBackgroundBrush(const QBrush& brush);
    QBrush backgroundBrush() const;

    void setBlurMethod(BlurMethod blurMethod);
    BlurMethod blurMethod() const;

    void setRegion(const QRegion& sourceRegion);
    const QRegion& region() const;

    void setBlurRadius(int radius);
    int blurRadius() const;

    void setDownsampleFactor(double factor);
    double downsampleFactor() const;

    void setMaxThreadCount(int nthreads);
    int maxThreadCount() const;

    void setCoordinateSystem(Qt::CoordinateSystem coordSystem);
    Qt::CoordinateSystem coordinateSystem() const;

protected:
    void draw(QPainter *painter) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void blurRadiusChanged(int);
    void blurOpacityChanged(double);
    void sourceOpacityChanged(double);
    void downsampleFactorChanged(double);
    void backgroundBrushChanged(const QBrush&);
    void repaintRequired();

private:
    QT_PIMPL(QtBlurBehindEffect)
};

