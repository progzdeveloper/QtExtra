#ifndef WIDGET_H
#define WIDGET_H

#include <QScopedPointer>
#include <QFrame>
#include <QPen>

#include <QtPaintUtils>

class QTabWidget;
class QPushButton;

class QtPropertyWidget;
class QtAttributeResource;

class PaintArea;

class Widget :
        public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private:
    void initUi();

    void createTextPage(QtAttributeResource* resource);
    void createRounderPage(QtAttributeResource* resource);
    void createStarPage(QtAttributeResource* resource);
private:
    QTabWidget* tabWidget;
    QScopedPointer<QtAttributeResource> mAttributeResource;
};



class PaintArea :
        public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(TextShape textShape READ textShape WRITE setTextShape)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
    Q_PROPERTY(QFont textFont READ textFont WRITE setTextFont)

    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)
    Q_PROPERTY(int penWidth READ penWidth WRITE setPenWidth)

    Q_PROPERTY(qreal stretch READ stretchFactor WRITE setStretchFactor)
    Q_PROPERTY(qreal angle READ startAngle WRITE setStartAngle)
    Q_PROPERTY(qreal offset READ offsetFactor WRITE setOffsetFactor)
    Q_PROPERTY(bool clockwise READ isClockwise WRITE setClockwise)

public:
    PaintArea(QWidget* parent = 0);

    enum TextShape {
        Curve,
        Ellipse
    };
    Q_ENUM(TextShape)

    void setTextShape(TextShape s);
    TextShape textShape() const;

    void setClockwise(bool on);
    bool isClockwise() const;

    void setStartAngle(qreal p);
    qreal startAngle() const;

    void setStretchFactor(qreal f);
    qreal stretchFactor() const;

    void setOffsetFactor(qreal f);
    qreal offsetFactor() const;

    void setText(const QString& text);
    QString text() const;

    void setTextColor(const QColor& c);
    QColor textColor() const;

    void setTextFont(const QFont& f);
    QFont textFont() const;

    void setPenColor(const QColor& c);
    QColor penColor() const;

    void setPenWidth(int w);
    int penWidth() const;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QString mText;
    TextShape mShape;
    qreal mAngle;
    qreal mStretchFactor;
    qreal mOffsetFactor;
    QPen mTextPen;
    QPen mLinePen;
    bool mClockwise;
};


class PolygonArea :
        public QFrame
{
    Q_OBJECT

    Q_PROPERTY(Shape shape READ shape WRITE setShape)
    Q_PROPERTY(double radius READ radius WRITE setRadius)
    Q_PROPERTY(QColor shapeColor READ shapeColor WRITE setShapeColor)
    Q_PROPERTY(bool outlineVisible READ isOutlineVisible WRITE setOutlineVisible)
public:
    enum Shape
    {
        Triangle,
        Rectangle,
        Star
    };
    Q_ENUM(Shape)

    explicit PolygonArea(QWidget* parent = Q_NULLPTR);

    void setShape(Shape s);
    Shape shape() const;

    void setShapeColor(QColor color);
    QColor shapeColor() const;

    void setRadius(double radius);
    double radius() const;

    void setOutlineVisible(bool on);
    bool isOutlineVisible() const;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    void updatePolygon(const QRect& rect);
    void updatePath();

private:
    QtPolygonRounder<ManhattanDistance> rounder;
    QPainterPath mPath;
    QPolygonF mPolygon;
    QColor mColor;
    double mRadius;
    Shape mShape;
    bool mOutline;
};

class StarArea :
        public QFrame
{
    Q_OBJECT

    Q_PROPERTY(int sideCount READ sideCount WRITE setSideCount)
    Q_PROPERTY(double factor READ factor WRITE setFactor)
    Q_PROPERTY(double radius READ radius WRITE setRadius)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    explicit StarArea(QWidget* parent);

    void setColor(QColor color);
    QColor color() const;

    void setRadius(double radius);
    double radius() const;

    void setFactor(double factor);
    double factor() const;

    void setSideCount(int sideCount);
    int sideCount() const;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    void updatePolygon();
    void updatePath();

private:
    QtPolygonRounder<ManhattanDistance> rounder;
    QPainterPath mPath;
    QPolygonF mPolygon;
    QColor mColor;
    quint32 mSideCount;
    double mFactor;
    double mRadius;
};


#endif // WIDGET_H
