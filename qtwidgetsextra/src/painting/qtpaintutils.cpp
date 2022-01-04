#include "qtpaintutils.h"


inline void __drawCircularText(QPainter& painter, const QString &text, const QPainterPath &path, qreal stretch, qreal start, qreal factor)
{
    QPen pen = painter.pen();

    QFontMetricsF metrics(painter.fontMetrics());
    qreal offset = metrics.height() * factor;

    qreal percentIncrease = qreal(1) / (qreal)(text.size() + 1) * stretch;

    for ( auto it = text.begin(); it != text.end(); ++it )
    {
        start += percentIncrease;
        if(start > 1.0)
        {
            //qDebug() << "start_in_percent is over 1.0:" << start_in_percent;
            start -= 1.0;
        }
        QPointF point = path.pointAtPercent(start);
        qreal angle = path.angleAtPercent(start);   // Clockwise is negative

        painter.save();
        // Move the virtual origin to the point on the curve
        painter.translate(point);
        // Rotate to match the angle of the curve
        // Clockwise is positive so we negate the angle from above
        painter.rotate(-angle);
        // Translate rotated painter down to half of text height
        painter.translate(0, offset);
        // Draw a line width above the origin to move the text above the line
        // and let Qt do the transformations
        painter.drawText(QPoint(0, -pen.width()), QString(*it));
        painter.restore();
    }
}


inline void __drawCurvedText(QPainter& painter, const QString &text, const QPainterPath &path, qreal stretch, qreal factor)
{
    QPen pen = painter.pen();

    qreal percentIncrease = qreal(1) / (qreal)(text.size() + 1) * stretch;
    qreal percent = 0;

    QFontMetricsF metrics(painter.fontMetrics());
    qreal offset = metrics.height() * factor;

    for ( auto it = text.begin(); it != text.end(); ++it )
    {
        //qreal offset = metrics.boundingRect(text[i]).height() * 0.5;

        percent += percentIncrease;

        QPointF point = path.pointAtPercent(percent);
        qreal angle = path.angleAtPercent(percent);   // Clockwise is negative

        painter.save();
        // Move the virtual origin to the point on the curve
        painter.translate(point);
        // Rotate to match the angle of the curve
        // Clockwise is positive so we negate the angle from above
        painter.rotate(-angle);
        // Translate rotated painter down to half of text height
        painter.translate(0, offset);
        // Draw a line width above the origin to move the text above the line
        // and let Qt do the transformations
        painter.drawText(QPoint(0, -pen.width()),QString(*it));
        painter.restore();
    }
}





QtPainter::QtPainter() :
    QPainter()
{
}

QtPainter::QtPainter(QPaintDevice *device) :
    QPainter(device)
{
}

QtPainter::~QtPainter()
{
}

void QtPainter::drawCurvedText(const QString &text,
                               const QPainterPath &path,
                               qreal stretch,
                               qreal factor)
{
    __drawCurvedText(*this, text, path,
                     qBound(qreal(0), stretch, qreal(1)),
                     qBound(qreal(0), factor, qreal(1)));
}

void QtPainter::drawCircularText(const QString &text,
                                 qreal x, qreal y,
                                 qreal w, qreal h,
                                 bool clockwise,
                                 qreal stretch,
                                 qreal start,
                                 qreal factor)
{
    QPainterPath path;
    path.addEllipse(x, y, w, h);
    if (!clockwise)
        path = path.toReversed();

    __drawCircularText(*this, text, path,
                       qBound(qreal(0), stretch, qreal(1)),
                       qBound(qreal(0), start, qreal(1)),
                       qBound(qreal(0), factor, qreal(1)));
}

void QtPainter::drawCircularText(const QString &text,
                                 const QPointF &p,
                                 const QSizeF &size,
                                 bool clockwise,
                                 qreal stretch,
                                 qreal start,
                                 qreal factor)
{
    QPainterPath path;
    path.addEllipse(p.x(), p.y(), size.width(), size.height());
    if (!clockwise)
        path = path.toReversed();

    __drawCircularText(*this, text, path,
                       qBound(qreal(0), stretch, qreal(1)),
                       qBound(qreal(0), start, qreal(1)),
                       qBound(qreal(0), factor, qreal(1)));
}

void QtPainter::drawCircularText(const QString &text,
                                 const QRectF &rect,
                                 bool clockwise,
                                 qreal stretch,
                                 qreal start,
                                 qreal factor)
{

    QPainterPath path;
    path.addEllipse(rect.x(), rect.y(), rect.width(), rect.height());
    if (!clockwise)
        path = path.toReversed();
    __drawCircularText(*this, text, path,
                       qBound(qreal(0), stretch, qreal(1)),
                       qBound(qreal(0), start, qreal(1)),
                       qBound(qreal(0), factor, qreal(1)));
}






QtPainterSaver::QtPainterSaver( QPainter * p )
    : painter( p )
{
    if ( painter )
        painter->save();
}

QtPainterSaver::QtPainterSaver( QPainter & p )
    : painter( &p )
{
    p.save();
}

QtPainterSaver::~QtPainterSaver() {
    if ( painter )
        painter->restore();
}
