#ifndef QTSTATEINDICATOR_H
#define QTSTATEINDICATOR_H

#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QFrame>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtStateIndicator :
        public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(Shape shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    Q_PROPERTY(bool clickable READ isClickable WRITE setClickable NOTIFY clickableChanged)

    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(qreal frameWidth READ frameWidth WRITE setFrameWidth NOTIFY frameWidthChanged)
    Q_PROPERTY(QFrame::Shadow shadow READ shadow WRITE setShadow NOTIFY shadowChanged)

public:
    enum Shape
    {
        Diamond,
        Rect,
        Round,
        Circle
    };
    Q_ENUM(Shape)

    explicit QtStateIndicator(QWidget* parent = Q_NULLPTR);
    explicit QtStateIndicator(const QColor& color, Shape shape = Rect, QWidget* parent = Q_NULLPTR);
    virtual ~QtStateIndicator();


    QSize minimumSizeHint() const;
    int heightForWidth(int w) const;

    Shape shape() const;

    QColor color() const;

    bool isClickable() const;

    bool isAnimated() const;

    int duration() const;

    qreal frameWidth() const;

    QFrame::Shadow shadow() const;


public Q_SLOTS:
    void setShape(Shape s);
    void setColor(const QColor& c);
    void setClickable(bool on = true);
    void setAnimated(bool on = true);
    void setDuration(int msec);
    void setFrameWidth(qreal w);
    void setShadow(QFrame::Shadow shadow);

Q_SIGNALS:
    void colorChanged(const QColor&);
    void clickableChanged(bool);
    void animatedChanged(bool);
    void blinkingChanged(bool);
    void durationChanged(int);
    void frameWidthChanged(qreal);
    void shadowChanged(QFrame::Shadow);
    void shapeChanged(Shape);

    // QObject interface
protected:
    virtual void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

    // QAbstractButton interface
protected:
    virtual bool hitButton(const QPoint &pos) const Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtStateIndicator)
};

#endif // QTSTATEINDICATOR_H
