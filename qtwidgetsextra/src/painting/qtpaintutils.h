#pragma once

#include <QtMath>

#include <QtWidgetsExtra>

#include <QPainter>
#include <QPainterPath>


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


struct EuclidDistance
{
    // precise, but not fast (since we have a square root computation)
    inline double operator()(const QPointF& p1, const QPointF& p2) const
    {
        const double dx = p1.x() - p2.x();
        const double dy = p1.y() - p2.y();
        const double d = dx * dx + dy * dy;
        return std::sqrt(d);
    }
};

struct ManhattanDistance
{
    // fast, but less accurate than EuclidianDistance
    inline double operator()(const QPointF& p1, const QPointF& p2) const
    {
        return std::abs(p1.x() - p2.x()) + std::abs(p1.y() - p2.y());
    }
};


template<class _Distance = ManhattanDistance>
class QtPolygonRounder : public _Distance
{
public:

    QPainterPath operator()(const QRect& rect, double radius) const {
        return operator ()(QRectF(rect), radius);
    }

    QPainterPath operator()(const QPolygonF& polygon, double radius) const
    {
        QPainterPath path;

        const int n = polygon.size();
        if (n < 3) // unable to rounding a polygon with less than 3 vertices
        {
            path.addPolygon(polygon);
            return path;
        }

        radius = std::max(0.0, radius); // radius can't be negative

        //
        // Next piece of code is an optimized
        // version of code given in comments below.
        // Optimized version eliminates modulo
        // operations and unnecessary computations,
        // so it looks more bizarre.
        //

        double r;
        QPointF pt1, pt2, curr, next;

        curr = polygon[0];
        next = polygon[1];

        // process first point
        r = distanceRadius(curr, next, radius);
        pt1 = lineStart(curr, next, r);
        path.moveTo(pt1);
        pt2 = lineEnd(curr, next, r);
        path.lineTo(pt2);

        int i = 1;
        for (; i < n - 1; ++i)
        {
            curr = polygon[i];
            next = polygon[i + 1];

            r = distanceRadius(curr, next, radius);
            pt1 = lineStart(curr, next, r);
            path.quadTo(curr, pt1);
            pt2 = lineEnd(curr, next, r);
            path.lineTo(pt2);
        }

        // process last point
        curr = polygon[i];
        next = polygon[0];

        r = distanceRadius(curr, next, radius);
        pt1 = lineStart(curr, next, r);
        path.quadTo(curr, pt1);
        pt2 = lineEnd(curr, next, r);
        path.lineTo(pt2);

        // close last corner
        curr = polygon[0];
        next = polygon[1];
        r = distanceRadius(curr, next, radius);
        pt1 = lineStart(curr, next, r);
        path.quadTo(curr, pt1);

        return path;

         /* simplified and non-optimized version of the above code
          *
          * for (int i = 0; i < n; ++i)
          * {
          *     curr = polygon[i];
          *     next = polygon[(i+1)%n];
          *     pt1 = lineStart(curr, next, distanceRadius(curr, next, radius));
          *     if (i == 0)
          *         path.moveTo(pt1);
          *     else
          *         path.lineTo(pt1);//path.quadTo(curr, pt1);
          *     pt2 = lineEnd(curr, next, distanceRadius(curr, next, radius));
          *     path.lineTo(pt2);
          * }
          *
          *  // close last corner
          * curr = polygon[0];
          * next = polygon[1];
          * pt1 = lineStart(curr, next, distanceRadius(curr, next, radius));
          * path.lineTo(pt1);//path.quadTo(curr, pt1);
          * return path;
          */
    }


    QPainterPath operator()(const QPolygonF& polygon, const QVector<double>& radiuses) const {
        return operator()(polygon, radiuses.data(), radiuses.size());
    }

    template<class _Alloc>
    QPainterPath operator()(const QPolygonF& polygon, const std::vector<double, _Alloc>& radiuses) const {
        return operator()(polygon, radiuses.data(), radiuses.size());
    }

    template<int _Prealloc>
    QPainterPath operator()(const QPolygonF& polygon, const QVarLengthArray<double, _Prealloc>& radiuses) const {
        return operator()(polygon, radiuses.data(), radiuses.size());
    }

    template<size_t _Size>
    QPainterPath operator()(const QRect& rect, const double (&radiuses)[_Size]) const {
        return operator()(QRectF(rect), radiuses, _Size);
    }


private:
    QPainterPath operator()(const QPolygonF& polygon, const double* radiuses, size_t length) const
    {
        QPainterPath path;
        const size_t n = static_cast<size_t>(polygon.size() - polygon.isClosed());
        length = std::min(length, n);
        if (n < 3 || !hasRadiuses(radiuses, length))
        {
            // unable to round off a polygon with less than 3 vertices
            // or if all radiuses is zero or if no radiuses was provided
            path.addPolygon(polygon);
            return path;
        }

        double r, radius0 = std::max(radiuses[0], 0.0);
        QPointF pt1, pt2, curr, next;

        curr = polygon[0];
        next = polygon[1];

        //process first point
        r = distanceRadius(curr, next, radius0);
        pt1 = lineStart(curr, next, r);
        path.moveTo(pt1);
        r = distanceRadius(curr, next, std::max(radiuses[1], 0.0));
        pt2 = lineEnd(curr, next, r);
        path.lineTo(pt2);

        size_t i = 1;
        for (; i < (n - 1); ++i)
        {
            curr = polygon[i];
            next = polygon[i + 1];
            if (i >= length)
            {
                path.lineTo(curr);
                path.lineTo(next);
                continue;
            }

            r = distanceRadius(curr, next, std::max(radiuses[i], 0.0));
            pt1 = lineStart(curr, next, r);
            path.quadTo(curr, pt1);
            r = distanceRadius(curr, next, std::max(radiuses[i + 1], 0.0));
            pt2 = lineEnd(curr, next, r);
            path.lineTo(pt2);
        }

        //process last point
        curr = polygon[i];
        next = polygon[0];

        r = distanceRadius(curr, next, std::max(radiuses[i], 0.0));
        pt1 = lineStart(curr, next, r);
        path.quadTo(curr, pt1);
        r = distanceRadius(curr, next, radius0);
        pt2 = lineEnd(curr, next, r);
        path.lineTo(pt2);

        // close last corner
        curr = polygon[0];
        next = polygon[1];
        r = distanceRadius(curr, next, radius0);
        pt1 = lineStart(curr, next, r);
        path.quadTo(curr, pt1);

        return path;
    }

    static bool hasRadiuses(const double* radiuses, size_t length)
    {
        if (length == 0)
            return true;

        for (const double* r = radiuses, *end = radiuses + length; r != end; ++r)
            if (!qFuzzyIsNull(*r))
                return true;

        return false;
    }

private:
    inline double distance(const QPointF& curr, const QPointF& next) const {
        return static_cast<const _Distance&>(*this)(curr, next);
    }

    inline double distanceRadius(const QPointF& curr, const QPointF& next, double radius) const {
        return std::min(0.5, radius / distance(curr, next));
    }

    inline QPointF lineStart(const QPointF& curr, const QPointF& next, double r) const
    {
        QPointF pt;
        pt.setX((1.0 - r) * curr.x() + r * next.x());
        pt.setY((1.0 - r) * curr.y() + r * next.y());
        return pt;
    }

    inline QPointF lineEnd(const QPointF& curr, const QPointF& next, double r) const
    {
        QPointF pt;
        pt.setX(r * curr.x() + (1.0 - r) * next.x());
        pt.setY(r * curr.y() + (1.0 - r) * next.y());
        return pt;
    }
};


struct QtStarPolygonizer
{
    inline QPolygonF operator()(int sideCount, double factor, const QRectF& rect) const
    {
        QPolygonF result;
        (*this)(result, sideCount, factor, rect);
        return result;
    }

    QPolygonF& operator()(QPolygonF& starGeometry, quint32 sideCount, double factor, const QRectF& rect) const;
};

struct QtStarPolygonizerF
{
    inline QPolygonF operator()(int sideCount, double factor, const QRectF& rect) const
    {
        QPolygonF result;
        (*this)(result, sideCount, factor, rect);
        return result;
    }

    QPolygonF& operator()(QPolygonF& starGeometry, quint32 sideCount, double factor, const QRectF& rect) const;
};
