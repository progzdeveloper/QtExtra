#include "previewlabel.h"

#include <QPaintEvent>
#include <QPainter>

#include <QStyleOption>

#include <QDebug>

#include <QElapsedTimer>

class PreviewLabelPrivate
{
public:
    QPixmap overlay; // overlay shadow pixmap
    QPixmap pixmap; // source pixmap
    QPainterPath maskPath; // clipping mask
    PreviewLabel::MaskShape shape; // clipping shape
    int frame; // current animation frame number
    bool loading; // flag for displaying placeholder animation

    PreviewLabelPrivate() : frame(0), loading(false){}

    int minSide(const QRect& r) { return std::min(r.width(), r.height()); }

    // update clipping mask
    void updateMask(const QRect& rect)
    {
#if (QT_VERSION > QT_VERSION_CHECK(5, 9, 1))
        maskPath.clear(); // this allows the path to reuse previous memory allocations
#else
        maskPath = QPainterPath();
#endif
        switch(shape)
        {
        case PreviewLabel::MaskShape::Circle:
            maskPath.addEllipse(rect); // create ellipse clipping path
            break;
        case PreviewLabel::MaskShape::Rounded: // create rounded rectangle clipping path
            maskPath.addRoundedRect(rect, 16.0, 16.0);
            break;
        default: // does nothing by default
            break;
        }
    }

    // update overlay shadow effect
    void updateOverlay(const QRect& rect)
    {
        if (shape != PreviewLabel::MaskShape::Circle)
        {
            overlay = QPixmap(); // reset overlay
            return;
        }

        // draw overlay:
        // - render radial gradinet
        // - render conical gradient (semi-transparent)
        // - combine gradients with CompositionMode_DestinationIn
        overlay = QPixmap(rect.size());
        overlay.fill(Qt::transparent);

        QPainter painter(&overlay);
        painter.setRenderHint(QPainter::Antialiasing);

        const int r = minSide(rect) / 2;

        QPoint center = rect.center();

        QRadialGradient radialGradient(center, r, center);
        radialGradient.setColorAt(0, Qt::transparent);
        radialGradient.setColorAt(0.5, QColor(124, 124, 124, 4));
        radialGradient.setColorAt(0.8, QColor(124, 124, 124, 24));
        radialGradient.setColorAt(1, QColor(124, 124, 124, 124));

        QConicalGradient gradient(center, -45);
        gradient.setColorAt(0, Qt::black);
        gradient.setColorAt(0.5, Qt::white);
        gradient.setColorAt(1.0, Qt::black);

        painter.setPen(Qt::NoPen);
        painter.setBrush(gradient);
        painter.drawEllipse(center, r, r);

        // combine gradients
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.setPen(Qt::NoPen);
        painter.fillRect(rect, radialGradient);
    }
};

PreviewLabel::PreviewLabel(QWidget *parent)
    : QAbstractButton(parent)
    , d(std::make_unique<PreviewLabelPrivate>())
{
    setCursor(Qt::PointingHandCursor);
}

PreviewLabel::~PreviewLabel()
{
}

void PreviewLabel::setMaskShape(PreviewLabel::MaskShape shape)
{
    if (d->shape == shape)
        return;

    d->shape = shape;
    d->updateMask(rect().adjusted(0, 0, -1, -1));
    update();
}

PreviewLabel::MaskShape PreviewLabel::maskShape() const
{
    return d->shape;
}

void PreviewLabel::setPixmap(const QPixmap &pixmap, Qt::TransformationMode mode)
{
    d->pixmap = pixmap.scaled(size(), Qt::KeepAspectRatio, mode);
    update();
}

QPixmap PreviewLabel::pixmap() const
{
    return d->pixmap;
}

void PreviewLabel::setLoading(bool on)
{
    if (d->loading == on)
        return;

    d->loading = on;
    update();
}

bool PreviewLabel::isLoading() const
{
    return d->loading;
}

void PreviewLabel::updateProgress()
{
    ++d->frame;
    if (d->frame > 10)
        d->frame = 0;
}

void PreviewLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (d->shape != MaskShape::NoShape)
    {
        painter.setClipPath(d->maskPath); // apply mask to painter
        painter.fillRect(rect(), Qt::lightGray); // fill background
    }

    if (d->loading) // draw placeholder gradient on loading
    {
        QLinearGradient gradient(QPoint(), QPoint(width(), 0));
        gradient.setColorAt(0.0, Qt::white);
        gradient.setColorAt(d->frame / 10.0, Qt::darkGray);
        painter.fillRect(rect(), gradient);
    }
    else // otherwise draw source pixmap
    {
        int x = width() / 2 - d->pixmap.width() / 2;
        int y = height() / 2 - d->pixmap.height() / 2;
        painter.drawPixmap(x, y, d->pixmap);
    }

    if (testAttribute(Qt::WA_UnderMouse) && d->shape == MaskShape::Circle)
        painter.drawPixmap(0, 0, d->overlay); // draw shadow effect if mouse over
}

void PreviewLabel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // update mask and overlay on resize
    d->updateMask(rect().adjusted(0, 0, -1, -1));
    d->updateOverlay(rect());
    update();
}
