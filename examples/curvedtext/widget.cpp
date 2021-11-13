#include "widget.h"
#include <QTabWidget>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

#include <QFile>
#include <QPainter>
#include <QPen>

#include <QPropertyAnimation>
#include <QtPropertyWidget>
#include <QtAttributeResource>

#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QFile file(":/metadata.json");
    file.open(QFile::ReadOnly);
    mAttributeResource.reset(new QtJsonAttributeResource);
    if (!mAttributeResource->read(&file))
        qWarning() << "Failed to read attribute resource: " << mAttributeResource->errorString();

    tabWidget = new QTabWidget(this);
    createTextPage(mAttributeResource.data());
    createRounderPage(mAttributeResource.data());
    createStarPage(mAttributeResource.data());

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);
}

Widget::~Widget()
{
}

void Widget::createTextPage(QtAttributeResource* resource)
{
    QWidget* pageWidget = new QWidget(this);
    PaintArea* paintArea = new PaintArea(pageWidget);
    QtPropertyWidget* browser = new QtPropertyWidget(pageWidget);

    QPushButton* animateButton = new QPushButton(tr("Animate!"), pageWidget);

    QPropertyAnimation* animation = new QPropertyAnimation(pageWidget);
    animation->setTargetObject(paintArea);
    animation->setPropertyName("stretch");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setDuration(1000);
    connect(animateButton, SIGNAL(clicked()), animation, SLOT(start()));

    browser->setFinal(true);
    if (resource)
        browser->setResource(resource);
    browser->setObject(paintArea);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, pageWidget);
    splitter->addWidget(paintArea);
    splitter->addWidget(browser);

    QVBoxLayout* layout = new QVBoxLayout(pageWidget);
    layout->addWidget(splitter);
    layout->addWidget(animateButton);

    tabWidget->addTab(pageWidget, tr("Curved Text"));
}

void Widget::createRounderPage(QtAttributeResource* resource)
{
    QWidget* pageWidget = new QWidget(this);
    PolygonArea* paintArea = new PolygonArea(pageWidget);
    QtPropertyWidget* browser = new QtPropertyWidget(pageWidget);

    browser->setFinal(true);
    if (resource)
        browser->setResource(resource);
    browser->setObject(paintArea);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, pageWidget);
    splitter->addWidget(paintArea);
    splitter->addWidget(browser);

    QVBoxLayout* layout = new QVBoxLayout(pageWidget);
    layout->addWidget(splitter);

    tabWidget->addTab(pageWidget, tr("Rounded Polygon"));
}

void Widget::createStarPage(QtAttributeResource *resource)
{
    QWidget* pageWidget = new QWidget(this);
    StarArea* paintArea = new StarArea(pageWidget);
    QtPropertyWidget* browser = new QtPropertyWidget(pageWidget);
    browser->setFinal(true);
    if (resource)
        browser->setResource(resource);
    browser->setObject(paintArea);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, pageWidget);
    splitter->addWidget(paintArea);
    splitter->addWidget(browser);

    QVBoxLayout* layout = new QVBoxLayout(pageWidget);
    layout->addWidget(splitter);

    tabWidget->addTab(pageWidget, tr("Star Polygon"));
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
    setFrameStyle(QFrame::Box|QFrame::Plain);
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
    painter.fillRect(contentsRect(), Qt::white);

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


PolygonArea::PolygonArea(QWidget *parent) :
    QFrame(parent),
    mRadius(0.0),
    mShape(PolygonArea::Triangle)
{
    setFrameStyle(QFrame::Box|QFrame::Plain);
    setMinimumSize(32, 32);
    setShapeColor(Qt::red);
    setShape(Triangle);
}

void PolygonArea::setShape(PolygonArea::Shape s)
{
    mShape = s;
    updatePolygon(rect());
    updatePath();

    if (isVisible())
        update();
}

PolygonArea::Shape PolygonArea::shape() const
{
    return mShape;
}

void PolygonArea::setShapeColor(QColor color)
{
    mColor = color;
    if (isVisible())
        update();
}

QColor PolygonArea::shapeColor() const
{
    return mColor;
}

void PolygonArea::setRadius(double radius)
{
    mRadius = radius;
    updatePolygon(rect());
    updatePath();
    if (isVisible())
        update();
}

double PolygonArea::radius() const
{
    return mRadius;
}

void PolygonArea::setOutlineVisible(bool on)
{
    mOutline = on;
    if (isVisible())
        update();
}

bool PolygonArea::isOutlineVisible() const
{
    return mOutline;
}

void PolygonArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(contentsRect(), Qt::white);

    painter.fillPath(mPath, mColor);
    if (mOutline)
    {
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(QPen(Qt::black, 0, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawPolygon(mPolygon);
    }
}

void PolygonArea::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updatePolygon(rect());
    updatePath();
    update();
}

void PolygonArea::updatePolygon(const QRect &rect)
{
    QtStarPolygonizer polygonizer;
    mPolygon.clear();
    switch (mShape)
    {
    case Triangle:
        mPolygon << QPointF(10.0, 10.0) << QPointF(rect.width() * 0.5, rect.height()-10.0) << QPointF(rect.width()-10.0, 10.0);
        break;
    case Rectangle:
        mPolygon << QPointF(10.0, 10.0) << QPointF(rect.width() - 10.0, 10.0)
                 << QPointF(rect.width() - 10.0, rect.height() - 10.0) << QPointF(10.0, rect.height() - 10.0);
        break;
    case Star:
        polygonizer(mPolygon, 5, 0.5, rect);
        break;
    default:
        break;
    }
}

void PolygonArea::updatePath()
{
    mPath = QPainterPath();
    if (mRadius == 0)
        mPath.addPolygon(mPolygon);
    else
        mPath = rounder(mPolygon, mRadius);
}

StarArea::StarArea(QWidget *parent) :
    QFrame(parent),
    mColor(Qt::red),
    mSideCount(5),
    mFactor(0.5),
    mRadius(0.0)
{
}

void StarArea::setColor(QColor color)
{
    mColor = color;
    update();
}

QColor StarArea::color() const
{
    return mColor;
}

void StarArea::setRadius(double radius)
{
    mRadius = radius;
    updatePolygon();
    updatePath();
    update();
}

double StarArea::radius() const
{
    return mRadius;
}

void StarArea::setFactor(double factor)
{
    mFactor = factor;
    updatePolygon();
    updatePath();
    update();
}

double StarArea::factor() const
{
    return mFactor;
}

void StarArea::setSideCount(int sideCount)
{
    mSideCount = std::max(3, sideCount);
    updatePolygon();
    updatePath();
    update();
}

int StarArea::sideCount() const
{
    return mSideCount;
}

void StarArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);
    painter.fillPath(mPath, mColor);
}

void StarArea::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updatePolygon();
    updatePath();
    update();
}

void StarArea::updatePolygon()
{
    QtStarPolygonizer polygonizer;
    mPolygon = polygonizer(mPolygon, mSideCount, mFactor, rect());
}

void StarArea::updatePath()
{
    mPath = QPainterPath();
    if (mRadius == 0)
        mPath.addPolygon(mPolygon);
    else
        mPath = rounder(mPolygon, mRadius);
}
