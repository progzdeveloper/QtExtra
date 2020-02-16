#include "widget.h"
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

#include <QFile>
#include <QPainter>
#include <QPen>
#include <QtPainter>
#include <QPropertyAnimation>
#include <QtPropertyWidget>
#include <QtAttributeResource>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    initUi();
}

Widget::~Widget()
{
}

void Widget::initUi()
{

    paintArea = new PaintArea(this);
    browser = new QtPropertyWidget(this);

    animateButton = new QPushButton(tr("Animate!"), this);

    QPropertyAnimation* animation = new QPropertyAnimation(this);
    animation->setTargetObject(paintArea);
    animation->setPropertyName("stretch");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setDuration(1000);
    connect(animateButton, SIGNAL(clicked()), animation, SLOT(start()));

    QFile file(":/metadata.json");
    file.open(QFile::ReadOnly);
    QtJsonAttributeResource* resource = new QtJsonAttributeResource;
    if (resource->read(&file))
        browser->setResource(resource);

    browser->setObject(paintArea);
    browser->setFinal(true);


    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(paintArea);
    splitter->addWidget(browser);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(splitter);
    layout->addWidget(animateButton);
}






PaintArea::PaintArea(QWidget *parent) :
    QFrame(parent),
    mText("sample text"),
    mShape(Curve),
    mAngle(0.5),
    mStretchFactor(1.0),
    mOffsetFactor(0.5),
    mTextPen(Qt::darkBlue),
    mLinePen(Qt::darkRed),
    mClockwise(true)
{

}

void PaintArea::setText(const QString &text)
{
    mText = text;
    repaint();
}

QString PaintArea::text() const
{
    return mText;
}

void PaintArea::setTextShape(PaintArea::TextShape s)
{
    mShape = s;
    repaint();
}

PaintArea::TextShape PaintArea::textShape() const
{
    return mShape;
}

void PaintArea::setClockwise(bool on)
{
    mClockwise = on;
    repaint();
}

bool PaintArea::isClockwise() const
{
    return mClockwise;
}

void PaintArea::setStartAngle(qreal p)
{
    mAngle = p;
    repaint();
}

qreal PaintArea::startAngle() const
{
    return mAngle;
}

void PaintArea::setStretchFactor(qreal f)
{
    mStretchFactor = f;
    repaint();
}

qreal PaintArea::stretchFactor() const
{
    return mStretchFactor;
}

void PaintArea::setOffsetFactor(qreal f)
{
    mOffsetFactor = f;
    repaint();
}

qreal PaintArea::offsetFactor() const
{
    return mOffsetFactor;
}

void PaintArea::setTextColor(const QColor &c)
{
    mTextPen.setColor(c);
    repaint();
}

QColor PaintArea::textColor() const
{
    return mTextPen.color();
}

void PaintArea::setTextFont(const QFont &f)
{
    setFont(f);
    repaint();
}

QFont PaintArea::textFont() const
{
    return font();
}

void PaintArea::setPenColor(const QColor &c)
{
    mLinePen.setColor(c);
    repaint();
}

QColor PaintArea::penColor() const
{
    return mLinePen.color();
}

void PaintArea::setPenWidth(int w)
{
    mLinePen.setWidth(w);
    repaint();
}

int PaintArea::penWidth() const
{
    return mLinePen.width();
}


void PaintArea::paintEvent(QPaintEvent *)
{
    QtPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    if (mShape == Curve) {
        QPainterPath path(QPointF(0.0, 0.0));

        QPointF c1(width()*0.2,height()*0.8);
        QPointF c2(width()*0.8,height()*0.2);

        path.cubicTo(c1, c2, QPointF(width(), height()));

        painter.setPen(mLinePen);
        //draw the bezier curve
        painter.drawPath(path);


        painter.setPen(mTextPen);
        painter.setFont(font());

        // draw text
        painter.drawCurvedText(mText, path, mStretchFactor, mOffsetFactor);

    } else {
        QRect r = rect().adjusted(16, 16, -16, -16);

        painter.setPen(mLinePen);
        painter.drawEllipse(r);

        painter.setPen(mTextPen);
        painter.setFont(font());
        // draw text
        painter.drawCircularText(mText, r, mClockwise, mStretchFactor, mAngle, mOffsetFactor);
    }
}

