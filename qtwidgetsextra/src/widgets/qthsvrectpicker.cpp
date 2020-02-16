#include "qthsvrectpicker.h"

#include <QPainter>
#include <QImage>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QStyleOptionFrame>

#include <QDebug>

static const int wid = 360;
static const int hei = 256;


class QtHsvRectPickerPrivate
{
public:
    QtHsvRectPickerPrivate(QtHsvRectPicker *q);
    int hueFromX(int) const;
    int satFromY(int) const;
    int satToY(int) const;
    int hueToX(int) const;
    void buildPixmap();
    int minSat;
    int maxSat;
    int minHue;
    int maxHue;
    int cVal;
    QPoint pos;
    QPixmap pixmap;
    QSize size;
    QColor color;
    QtHsvRectPicker* q_ptr;
};

QtHsvRectPickerPrivate::QtHsvRectPickerPrivate(QtHsvRectPicker *q) :
    q_ptr(q)
{
    minSat = 0;
    maxSat = 255;
    minHue = 0;
    maxHue = 359;
    cVal = 200;
    pos = QPoint(0, 0);
}

#if 0
/*!
 * \internal
 */
void QtHsvRectPickerPrivate::buildPixmap()
{
    int cy = q_ptr->contentsRect().height();
    int cx = q_ptr->contentsRect().width();

    QImage img(cx, cy, QImage::Format_RGB32);
    for (int y = 0; y < cy; y++)
    {
        for (int x = 0; x < cx; x++) {
            QColor c;
            c.setHsv(hueFromX(x), satFromY(y), cVal);
            img.setPixel(x, y, c.rgb());
        }
    }
    pixmap = QPixmap::fromImage(img);
    size = pixmap.size();
}
#endif


void QtHsvRectPickerPrivate::buildPixmap()
{
    int cx = q_ptr->contentsRect().width();
    int cy = q_ptr->contentsRect().height();

    int hueDiff = (maxHue - minHue);
    int satDiff = (maxSat - minSat);
    QColor c;
    QImage img(cx, cy, QImage::Format_RGB32);
    for (int y = 0; y < cy; y++)
    {
        QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < cx; x++) {
            c.setHsv(maxHue - ((x * hueDiff) / cx), maxSat - ((y * satDiff) / cy), cVal);
            line[x] = c.rgb();
        }
    }
    pixmap = QPixmap::fromImage(img);
    size = pixmap.size();
}

/*!
 * \internal
 */
int QtHsvRectPickerPrivate::hueFromX(int x) const
{
    return maxHue - (x * (maxHue-minHue) / q_ptr->contentsRect().width());
}

/*!
 * \internal
 */
int QtHsvRectPickerPrivate::satFromY(int y) const
{
    return maxSat - (y * (maxSat-minSat) / q_ptr->contentsRect().height());
}

/*!
 * \internal
 */
int QtHsvRectPickerPrivate::satToY(int s) const
{
    float ys = q_ptr->contentsRect().height() * 1.0 / (maxSat-minSat);
    float dist = s * ys;
    return (int)(q_ptr->contentsRect().height() - dist);
}


/*!
 * \internal
 */
int QtHsvRectPickerPrivate::hueToX(int h) const
{
    float xs = q_ptr->contentsRect().width() * 1.0 / (maxHue-minHue); // pixels per hue
    float dist = h * xs;
    return (int)(q_ptr->contentsRect().width() - dist);
}



/*!
 *  \class QtHsvRectPicker
 *  \inmodule wwWidgets
 *  \brief The QtHsvRectPicker class provides a widget allowing color picking
 *         in a form of a hue-saturation rectangle.
 *
 *  \image qwwhuesatpicker.png QtHsvRectPicker
 *  \ingroup colorclasses
 */

/*!
 *  \property   QtHsvRectPicker::value
 *  \brief      This property holds the value component of the table visible
 *
 *  This property keeps the lightness component of the color table.
 *  The range of allowed values for this property is 0-255. The default value is 220.
 */

/*!
 *  \property   QtHsvRectPicker::color
 *  \brief      This property holds the currently chosen color.
 *
 *
 */

/*!
 *  \property   QtHsvRectPicker::minimumSat
 *  \brief      This property holds the minimum value of the saturation component shown by the widget.
 *
 *
 */

/*!
 *  \property   QtHsvRectPicker::maximumSat
 *  \brief      This property holds the maximum value of the saturation component shown by the widget.
 *
 *
 */

/*!
 *  \property   QtHsvRectPicker::minimumHue
 *  \brief      This property holds the minimum value of the hue component shown by the widget.
 *
 *
 */

/*!
 *  \property   QtHsvRectPicker::maximumHue
 *  \brief      This property holds the maximum value of the hue component shown by the widget.
 *
 *
 */
/*!
 *  \fn         QtHsvRectPicker::colorPicked(QColor color)
 *  \brief      This signal is emitted when \a color is picked.
 */
/*!
 *  \fn         QtHsvRectPicker::hueRangeChanged(int min, int max)
 *  \brief      This signal is emitted when the Hue range changes to (\a min, \a max)
 */

/*!
 * Constructs a hue sat picker with a given \a parent.
 */
QtHsvRectPicker::QtHsvRectPicker(QWidget *parent) :
    QFrame(parent),
    d_ptr(new QtHsvRectPickerPrivate(this))
{
    Q_D(QtHsvRectPicker);
    d->buildPixmap();
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

QtHsvRectPicker::~QtHsvRectPicker()
{
}


/*!
 *  \brief Sets the current value
 */
void QtHsvRectPicker::setValue(int v)
{
    Q_D(QtHsvRectPicker);
    if (d->cVal==v)
        return;

    if (v<0 || v>255)
        return;

    d->cVal = v;
    emit valueChanged(v);

    d->size=QSize();
    d->buildPixmap();
    update();
}

/*!
 * \internal
 */
QSize QtHsvRectPicker::sizeHint() const {
    Q_D(const QtHsvRectPicker);
    int w = (d->maxHue - d->minHue) + 2*frameWidth();
    int h = (d->maxSat - d->minSat) + 2*frameWidth();
    return QSize(w, h);
}

/*!
 * \internal
 */
QSize QtHsvRectPicker::minimumSizeHint() const {
    Q_D(const QtHsvRectPicker);
    int w = qMin( (d->maxHue - d->minHue) / 4, 40) + 2*frameWidth();
    int h = qMin( (d->maxSat - d->minSat) / 4, 40) + 2*frameWidth();
    return QSize(w, h);
}



/*!
 * \internal
 */
void QtHsvRectPicker::paintEvent(QPaintEvent *)
{
    Q_D(QtHsvRectPicker);
    QPainter p(this);
    drawFrame(&p);
    QRect rct = contentsRect();
    QStyleOptionFrame opt;
    opt.initFrom(this);
    if (opt.state & QStyle::State_Enabled) {
        p.drawPixmap(rct.topLeft(), d->pixmap);
        drawCrosshair(&p, d->pos);
    } else {
        QIcon i(d->pixmap);
        i.paint(&p, rct, 0, QIcon::Disabled);
    }
}



/*!
 * \internal
 */
void QtHsvRectPicker::mousePressEvent(QMouseEvent * e)
{
    Q_D(QtHsvRectPicker);
    if (e->button() == Qt::LeftButton) {
        QPoint pt = e->pos();
        int h = d->hueFromX(pt.x()-contentsRect().x());
        int s = d->satFromY(pt.y()-contentsRect().y());
        if (h<0 || s<0 || h>359 || s>255)
            return;

        d->pos = pt;
        d->color = QColor::fromHsv(h, s, d->cVal);
        emit colorChanged(d->color);

        update();
    } else
        QFrame::mousePressEvent(e);
}

/*!
 * \internal
 */
void QtHsvRectPicker::mouseMoveEvent(QMouseEvent * e)
{
    Q_D(QtHsvRectPicker);
    if (e->buttons() & Qt::LeftButton) {
        QPoint pt = e->pos();
        if (!contentsRect().contains(pt))
            return;
        int h = d->hueFromX(pt.x()-contentsRect().x());
        int s = d->satFromY(pt.y()-contentsRect().y());
        if (h<0 || s<0 || h>359 || s>255)
            return;

        d->pos = pt;
        d->color = QColor::fromHsv(h, s, d->cVal);
        emit colorChanged(d->color);

        update();
    } else
        QFrame::mouseMoveEvent(e);
}

/*!
 * \brief       This method draws the crosshair pointing the currently chosen color at point \a pt on painter \a p.
 */
void QtHsvRectPicker::drawCrosshair(QPainter * painter, const QPoint & pt)
{
    Q_D(QtHsvRectPicker);

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

/*!
 *
 */
void QtHsvRectPicker::setMinimumSat(int v)
{
    Q_D(QtHsvRectPicker);
    setSatRange(v, qMax(d->maxSat, v));
}

/*!
 *
 */
void QtHsvRectPicker::setMinimumHue(int h)
{
    Q_D(QtHsvRectPicker);
    setHueRange(h, qMax(d->maxHue, h));
}

/*!
 *
 */
void QtHsvRectPicker::setMaximumHue(int h)
{
    Q_D(QtHsvRectPicker);
    setHueRange(qMin(d->minHue, h), h);
}

/*!
 *
 */
void QtHsvRectPicker::setHueRange(int mi, int ma)
{
    Q_D(QtHsvRectPicker);
    if (mi<0 || ma < 0 || mi > 359 || ma>359 || mi >ma) return;
    bool maC = d->maxHue!=ma;
    bool miC = d->minHue!=mi;
    d->maxHue = ma;
    d->minHue = mi;
    if (maC) emit maximumHueChanged(ma);
    if (miC) emit minimumHueChanged(mi);
    if (miC || maC) emit hueRangeChanged(mi, ma);
    else return;

    updateGeometry();
    d->buildPixmap();
    update();
}

/*!
 *
 */
void QtHsvRectPicker::setMaximumSat(int v)
{
    Q_D(QtHsvRectPicker);
    setSatRange(qMin(d->minSat, v), v);
}

/*!
 *
 */
void QtHsvRectPicker::setSatRange(int mi, int ma)
{
    Q_D(QtHsvRectPicker);
    if (mi < 0 || ma < 0 || mi > 255 || ma > 255 || mi >ma)
        return;
    bool maC = d->maxSat!=ma;
    bool miC = d->minSat!=mi;
    d->maxSat = ma;
    d->minSat = mi;
    if (maC) emit maximumSatChanged(ma);
    if (miC) emit minimumSatChanged(mi);
    if (miC || maC) emit satRangeChanged(mi, ma);
    else return;
//     m_maxS = ma;
//     m_minS = mi;
    updateGeometry();
    d->buildPixmap();
    update();
}

/*!
 *
 */
void QtHsvRectPicker::setColor(const QColor & c)
{
    Q_D(QtHsvRectPicker);
    int h, s, v;
    if (d->color == c)
        return;

    c.getHsv(&h, &s, &v);
    if (d->cVal!=v)
        setValue(v);

    int Hspan = d->maxHue - d->minHue;
    int Sspan = d->maxSat - d->minSat;
    if (s < (d->minSat) || s>(d->maxSat)) {
        int ss = qMax(0, s-Sspan/2);
        setSatRange(ss, ss+Sspan);
    }
    if (h < (d->minHue) || h>(d->maxHue)) {
        int hh = qMax(0, h-Hspan/2);
        setHueRange(hh, hh+Hspan);
    }

    d->pos.setX(d->hueToX(h));
    d->pos.setY(d->satToY(s));
    d->color = c;
    emit colorChanged(c);

    update();
}



/*!
 * \internal
 */
void QtHsvRectPicker::resizeEvent(QResizeEvent *re)
{
    Q_D(QtHsvRectPicker);

    QFrame::resizeEvent(re);

    QSize s = re->size();
    float xs = s.width() * 1.0 / (d->maxHue - d->minHue); // pixels per hue
    float x = d->color.hue() * xs;

    float ys = s.height() * 1.0 / (d->maxSat - d->minSat); // pixels per sat
    float y = d->color.saturation() * ys;

    d->pos.setX((int)(s.width() - x));
    d->pos.setY((int)(s.height() - y));

    d->buildPixmap();

    update();
}

QColor QtHsvRectPicker::color() const
{
    Q_D(const QtHsvRectPicker);
    return d->color;
}


int QtHsvRectPicker::minimumSat() const
{
    Q_D(const QtHsvRectPicker);
    return d->minSat;
}

int QtHsvRectPicker::maximumSat() const
{
    Q_D(const QtHsvRectPicker);
    return d->maxSat;
}


int QtHsvRectPicker::maximumHue() const
{
    Q_D(const QtHsvRectPicker);
    return d->maxHue;
}


int QtHsvRectPicker::minimumHue() const
{
    Q_D(const QtHsvRectPicker);
    return d->minHue;
}


int QtHsvRectPicker::value() const
{
    Q_D(const QtHsvRectPicker);
    return d->cVal;
}

