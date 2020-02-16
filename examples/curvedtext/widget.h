#ifndef WIDGET_H
#define WIDGET_H

#include <QFrame>
#include <QPen>

class QPushButton;

class QtPropertyWidget;

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

private:
    QtPropertyWidget* browser;
    PaintArea* paintArea;
    QPushButton* animateButton;
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

#endif // WIDGET_H
