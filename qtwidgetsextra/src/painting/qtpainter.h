#ifndef QTPAINTER_H
#define QTPAINTER_H

#include <QtWidgetsExtra>

#include <QPainter>


class QTWIDGETSEXTRA_EXPORT QtPainter :
        public QPainter
{
public:
    QtPainter();
    explicit QtPainter(QPaintDevice *device);
    ~QtPainter();

    void drawCurvedText(const QString& text,
                        const QPainterPath& path,
                        qreal stretch = 0.5,
                        qreal factor = 0.5);

    void drawCircularText(const QString& text,
                          qreal x, qreal y,
                          qreal w, qreal h,
                          bool clockwise = true,
                          qreal stretch = 0.5,
                          qreal start = 0,
                          qreal factor = 0.5);

    void drawCircularText(const QString& text,
                          const QPointF &p,
                          const QSizeF &size,
                          bool clockwise = true,
                          qreal stretch = 0.5,
                          qreal start = 0,
                          qreal factor = 0.5);

    void drawCircularText(const QString& text,
                          const QRectF& rect,
                          bool clockwise = true,
                          qreal stretch = 0.5,
                          qreal start = 0,
                          qreal factor = 0.5);
};

/*!
 *
 * \class QtPainterSaver
 *
 * \brief Exception-safe and convenient wrapper around QPainter::save()
 * \link QPainter::restore() restore()\endlink
 *
 * This class automates the task of matching QPainter::save() with
 * QPainter::restore() calls. If you always use this class instead of
 * direct calls, you can never forget to call \link QPainter::restore() restore()\endlink.
 * This is esp. important when dealing with code that might throw
 * exceptions, or functions with many return statements.
 *
 */
class QTWIDGETSEXTRA_EXPORT QtPainterSaver
{
    Q_DISABLE_COPY( QtPainterSaver )
public:
    /*!
     * Constructor. Calls \link QPainter::save() save()\endlink on \a p.
     */
    explicit QtPainterSaver( QPainter * p );

    /*!
     * \overload
     */
    explicit QtPainterSaver( QPainter & p );
    /*!
     * Destructor. Calls \link QPainter::restore() restore()\endlink on the contained painter.
     */
    ~QtPainterSaver();

private:
    QPainter * painter;
};



#endif // QTPAINTER_H
