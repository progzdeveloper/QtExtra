#include <cmath>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>

#include "qthsvradialpicker.h"

#ifndef M_PI
#define M_PI            3.14159265358979323846  /* pi */
#endif

/**
 *  @internal
 */
class QtHsvRadialPickerPrivate {
public:
    QtHsvRadialPickerPrivate(QtHsvRadialPicker *q);

    QPixmap pixmap;
    QConicalGradient gradient;
    QtHsvRadialPicker* q_ptr;
    QPoint pos;
    QColor color;
    int cValue;

    void buildPixmap();
    int radius(const QPoint &pt) const;
    int hue(const QPoint &pt) const;


};

QtHsvRadialPickerPrivate::QtHsvRadialPickerPrivate(QtHsvRadialPicker *q) :
    q_ptr(q)
{
    const qreal factor = 1.0 / 360;
    cValue = 255;

    for (int i = 0; i < 360; i++) {
        gradient.setColorAt(i * factor, QColor::fromHsv(i, 255, cValue));
    }
    gradient.setColorAt(1, QColor::fromHsv(359, 255, cValue));

}

void QtHsvRadialPickerPrivate::buildPixmap()
{
    int n = qMin(q_ptr->width(), q_ptr->height());
    QSize size(n, n);
    QRect rect(0, 0, n, n);

    // build huecircle
    gradient.setCenter(rect.center());

    QImage huecircle(size, QImage::Format_ARGB32);
    huecircle.fill(Qt::transparent);

    QPainter phcircle(&huecircle);
    phcircle.setRenderHint(QPainter::Antialiasing, true);
    phcircle.setBrush(QBrush(gradient));
    phcircle.setPen(q_ptr->palette().color(QPalette::Shadow));
    phcircle.drawEllipse(rect.adjusted(1, 1, -1, -1));
    pixmap = QPixmap::fromImage(huecircle);

    // alpha gradient
    QRadialGradient rg(rect.center(), size.width() / 2 - 2, rect.center());

    for (qreal i = 0 ; i < 1; i += 0.1) {
        rg.setColorAt(i, QColor::fromHsv(0, 0, (int)(256*i)));
        //rg.setColorAt(i, QColor::fromHsvF(0, 0, i));
    }
    rg.setColorAt(1, Qt::white);

#if QT_VERSION < 0x050000
    // alpha channel
    QImage ac(q->size(), QImage::Format_RGB32);
    ac.fill(Qt::transparent);
    QPainter acpainter(&ac);
    acpainter.setPen(Qt::NoPen);
    acpainter.setBrush(QBrush(rg));
    acpainter.drawEllipse(q->rect().adjusted(1, 1, -1, -1));
    px.setAlphaChannel(QPixmap::fromImage(ac));
#endif
    // destination image
    QImage dst(size, QImage::Format_ARGB32);
    dst.fill(Qt::transparent);

    QPainter dstp(&dst);
    dstp.setBrush(QColor::fromHsv(0, 0, cValue));
    dstp.setRenderHint(QPainter::Antialiasing, true);
    dstp.setPen(q_ptr->palette().color(QPalette::Shadow));
    dstp.drawEllipse(rect.adjusted(1, 1, -1, -1));
    dstp.setCompositionMode(QPainter::CompositionMode_SourceOver);
    dstp.drawPixmap(0, 0, pixmap);
    /*dstp.setCompositionMode(QPainter::CompositionMode_Source);
    dstp.setBrush(Qt::NoBrush);
    dstp.setPen(Qt::black);
    dstp.drawEllipse(rect()); */
    pixmap = QPixmap::fromImage(dst);
}

int QtHsvRadialPickerPrivate::radius(const QPoint & pt) const
{
    QPoint c = q_ptr->rect().center();
    int x = pt.x() - c.x();
    int y = pt.y() - c.y();
    int r = qRound(sqrt((double)(x*x + y*y)));

    return r;
}

int QtHsvRadialPickerPrivate::hue(const QPoint & pt) const
{
    QPoint c = q_ptr->rect().center();
    int x = c.x() - pt.x();
    int y = pt.y() - c.y();
    qreal a = qAbs(y) * 1.0 / qAbs(x);   // tangent
    qreal rd = atan(a);
    qreal deg = rd * 180.0 / M_PI;
    int h = qRound(deg);
    if (x>=0 && y>=0)
        h= 180 + h;
    else if (x<0 && y>=0)
        h= 360- h;
    else if (x<0 && y<0)
        h = h;
    else
        h = 180 - h;

    return h % 360;
}

/*!
 *  \class QtHsvRadialPicker
 *  \inmodule wwWidgets
 *  \brief The QtHsvRadialPicker class provides a widget allowing color picking
 *         in a form of hue-saturation circle.
 *
 *  \image qwwhuesatradialpicker.png QtHsvRadialPicker
 *
 *  \ingroup colorclasses
 */
/*!
 *  \fn     QtHsvRadialPicker::valueChanged(int value)
 *  \brief  This signal is emitted whenever the V component of the color changes to \a value.
 */
/*!
 *  \fn     QtHsvRadialPicker::colorPicked(QColor color)
 *  \brief  This signal is emitted whenever \a color is picked.
 */

/*!
 * Constructs a radial picker widget with a given \a parent.
 *
 */
QtHsvRadialPicker::QtHsvRadialPicker(QWidget *parent)
        : QWidget(parent), d_ptr(new QtHsvRadialPickerPrivate(this))
{
    setMinimumSize(minimumSizeHint());
}

QtHsvRadialPicker::~QtHsvRadialPicker()
{
}

void QtHsvRadialPicker::setValue(int v)
{
    Q_D(QtHsvRadialPicker);
    if (v < 0 || v > 255 || v == d->cValue)
        return;

    d->cValue = v;
    emit valueChanged(v);

    d->gradient.setStops(QGradientStops());
    for (int i = 0; i < 360; i++) {
        d->gradient.setColorAt(i * 1.0/360, QColor::fromHsv(i, 255, d->cValue));
    }
    d->gradient.setColorAt(1, QColor::fromHsv(359, 255, d->cValue));
    d->buildPixmap();
    update();
}

/*!
 * \property QtHsvRadialPicker::value
 * \brief  Property holds the Value component of colors available in the picker.
 *
 */
int QtHsvRadialPicker::value() const {
    Q_D(const QtHsvRadialPicker);
    return d->cValue;
}

/*!
 *  \internal
 */
QSize QtHsvRadialPicker::minimumSizeHint() const {
    return QSize(128, 128);
}


/*!
 *  \internal
 */
void QtHsvRadialPicker::paintEvent(QPaintEvent *)
{
    Q_D(QtHsvRadialPicker);
    if (d->pixmap.isNull())
        d->buildPixmap();

    QPainter p(this);
    p.drawPixmap(0, 0, d->pixmap);
    drawCrosshair(&p, d->pos);
}


void QtHsvRadialPicker::showEvent(QShowEvent *e)
{
    Q_D(QtHsvRadialPicker);
    QWidget::showEvent(e);
    if (width()!=height()) {
        int s = qMin(width(), height());
        resize(s, s);
        qApp->processEvents();
        return;
    }
    d->buildPixmap();
    update();
}

/*!
 *  \internal
 */
void QtHsvRadialPicker::resizeEvent(QResizeEvent *e)
{
    Q_D(QtHsvRadialPicker);
    QWidget::resizeEvent(e);
    if (width()!=height()) {
        int s = qMin(e->size().width(), e->size().height());
        resize(s, s);
        qApp->processEvents();
        return;
    }

    int h, s, v;
    d->color.getHsv(&h, &s, &v);

    qreal p = h * M_PI / 180.0;
    int r = qRound(s * 1.0 * (width() / 2 - 2) / 255.0);
    int x = qRound(r * cos(p));
    int y = qRound(r * sin(p));

    QPoint ctr = rect().center();
    d->pos = QPoint(x + ctr.x(), -y + ctr.y());

    d->buildPixmap();
    update();
}


/*!
 *  \internal
 */
void QtHsvRadialPicker::mousePressEvent(QMouseEvent * e)
{
    Q_D(QtHsvRadialPicker);
    int w = width();
    int halfWidth = w / 2;
    if (e->button()==Qt::LeftButton) {
        QPoint pt = e->pos();
        int r = d->radius(pt);
        if (r >= halfWidth)
            return;

        int h = d->hue(pt);
        int s = (int)(r * 255.0 / (halfWidth));

        d->pos = pt;
        d->color = QColor::fromHsv(h, s, d->cValue);
        emit colorChanged(d->color);
        update();
    } else
        QWidget::mousePressEvent(e);
}

/*!
 *  \internal
 */
void QtHsvRadialPicker::mouseMoveEvent(QMouseEvent * me)
{
    Q_D(QtHsvRadialPicker);
    if (me->buttons() & Qt::LeftButton)
    {
        int w = width();
        int halfWidth = w / 2;
        QPoint pt = me->pos();
        int r = d->radius(pt);
        if (r >= halfWidth) {
            r = halfWidth;
            pt = d->pos;
        }

        int h = d->hue(pt);
        int s = (int)(r * 255.0 / (halfWidth));
        d->pos = pt;
        d->color = QColor::fromHsv(h, s, d->cValue);

        emit colorChanged(d->color);
        update();
    } else
        QWidget::mouseMoveEvent(me);
}

/*!
 *  \internal
 *
 */
void QtHsvRadialPicker::changeEvent(QEvent * ce)
{
    Q_D(QtHsvRadialPicker);
    if (ce->type() == QEvent::PaletteChange ||
        ce->type() == QEvent::ApplicationPaletteChange)
    {
        d->buildPixmap();
        update();
    }
    QWidget::changeEvent(ce);
}



/*!
 *  \brief  Draws the crosshair on painter \a p at point \a pt.
 *
 *
 */
void QtHsvRadialPicker::drawCrosshair(QPainter * painter, const QPoint & pt)
{
    Q_D(QtHsvRadialPicker);
    if (pt.isNull())
        return;

    int ri = d->color.red();
    int gi = d->color.green();
    int bi = d->color.blue();

    painter->save();
    // Find a color for painting the selector based on the brightness
    // value of the color.
    if ((ri * 30) + (gi * 59) + (bi * 11) > 12800)
        painter->setPen(QPen(Qt::black));
    else
        painter->setPen(QPen(Qt::white));
    painter->drawLine(pt-QPoint(0, -6), pt-QPoint(0, -2));
    painter->drawLine(pt-QPoint(0, 2), pt-QPoint(0, 6));
    painter->drawLine(pt-QPoint(-6, 0), pt-QPoint(-2, 0));
    painter->drawLine(pt-QPoint(2, 0), pt-QPoint(6, 0));
    painter->restore();
}

void QtHsvRadialPicker::setColor(const QColor & c)
{
    Q_D(QtHsvRadialPicker);
    if (c == d->color)
        return;

    int h, s, v;
    c.getHsv(&h, &s, &v);
    if (v != d->cValue)
        setValue(v);

    qreal p = h * M_PI / 180.0;
    int r = qRound(s * 1.0 * (width() / 2 - 2) / 255.0);
    int x = qRound(r * cos(p));
    int y = qRound(r * sin(p));

    QPoint ctr = rect().center();
    d->pos = QPoint(x + ctr.x(), -y + ctr.y());
    d->color = c;
    emit colorChanged(c);

    update();
}

/*!
 * \property QtHsvRadialPicker::color
 * \brief  Property holds the currently picked color.
 *
 */
const QColor QtHsvRadialPicker::color() const {
    Q_D(const QtHsvRadialPicker);
    return d->color;
}

