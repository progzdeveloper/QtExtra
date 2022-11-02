#include "widget.h"
#include "qtradiusesmodel.h"
#include <memory>
#include <QItemEditorFactory>
#include <QTabWidget>
#include <QTableView>
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

class MyItemEditorFactory : public QItemEditorFactory
{
public:
    virtual QWidget* createEditor(int userType, QWidget *parent) const
    {
        if (userType == QVariant::Double)
        {
            QSlider* slider = new QSlider(Qt::Horizontal, parent);
            slider->setAutoFillBackground(true);
            return slider;
        }
        return QItemEditorFactory::createEditor(userType, parent);
    }
};


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QItemEditorFactory::setDefaultFactory(new MyItemEditorFactory);

    QFile file(":/metadata.json");
    file.open(QFile::ReadOnly);
    mAttributeResource.reset(new QtJsonAttributeResource);
    if (!mAttributeResource->read(&file))
        qWarning() << "Failed to read attribute resource: " << mAttributeResource->errorString();

    tabWidget = new QTabWidget(this);
    createTextPage(mAttributeResource.data());
    createRounderPage(mAttributeResource.data());
    createStarPage(mAttributeResource.data());
    createPathPage(mAttributeResource.data());

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


    QTableView* view = new QTableView(pageWidget);
    QtRadiusesModel* model = new QtRadiusesModel(view);
    view->setModel(model);

    QWidget* browserWidget = new QWidget(pageWidget);
    QVBoxLayout* browserLayout = new QVBoxLayout(browserWidget);
    browserLayout->addWidget(browser);
    browserLayout->addWidget(view);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, pageWidget);
    splitter->addWidget(paintArea);
    splitter->addWidget(browserWidget);

    QVBoxLayout* layout = new QVBoxLayout(pageWidget);
    layout->addWidget(splitter);

    tabWidget->addTab(pageWidget, tr("Star Polygon"));

    model->reset(paintArea->sideCount() * 2);
    connect(model, &QtRadiusesModel::dataChanged, this, [paintArea, model]()
    {
        paintArea->setRadiuses(model->radiuses());
    });

    connect(paintArea, &StarArea::verticesCountChanged, this, [paintArea, model](int n)
    {
        model->reset(n);
        paintArea->setRadiuses(model->radiuses());
    });
}

void Widget::createPathPage(QtAttributeResource *resource)
{
    QWidget* pageWidget = new QWidget(this);
    PathArea* paintArea = new PathArea(pageWidget);
    paintArea->setContentsMargins(10, 10, 10, 10);
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

    tabWidget->addTab(pageWidget, tr("Path Polygon"));
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


bool hasNonZeroRadiuses(const double* radiuses, size_t length)
{
    if (length == 0)
        return true;

    for (const double* r = radiuses, *end = radiuses + length; r != end; ++r)
        if (!qFuzzyIsNull(*r))
            return true;

    return false;
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
    if (mRadius == radius)
        return;

    mRadius = radius;
    updatePolygon();
    updatePath();
    update();
}

double StarArea::radius() const
{
    return mRadius;
}

void StarArea::setRadiuses(const RadiusesVector& radiuses)
{
    mRadiuses = radiuses;

    updatePath();
    update();
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
    sideCount = std::max(3, sideCount);
    if (mSideCount == sideCount)
        return;

    mSideCount = sideCount;
    updatePolygon();
    updatePath();
    update();

    Q_EMIT verticesCountChanged(mPolygon.size());
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
    QtPolygonRounder<ManhattanDistance> rounder;
    mPath = QPainterPath();
    if (mRadius == 0 && hasNonZeroRadiuses(mRadiuses.data(), mRadiuses.size()))
        mPath = rounder(mPolygon,  mRadiuses);
    else
        mPath = rounder(mPolygon, mRadius);
}



PathArea::PathArea(QWidget *parent)
{
}

void PathArea::setEdges(Qt::Edges e)
{
    if (mEdges == e)
        return;

    mEdges = e;
    updatePath();
    update();
}

Qt::Edges PathArea::edges() const
{
    return mEdges;
}

void PathArea::setArrowSize(const QSize &s)
{
    if (mArrowSize == s)
        return;

    mArrowSize = s;
    updatePath();
    update();
}

QSize PathArea::arrowSize() const
{
    return mArrowSize;
}

void PathArea::setRadius(double radius)
{
    if (mRadius == radius)
        return;

    mRadius = radius;
    updatePath();
    update();
}

double PathArea::radius() const
{
    return mRadius;
}

void PathArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);
    painter.fillPath(mPath, Qt::red);
}

void PathArea::resizeEvent(QResizeEvent *event)
{
    updatePath();
    update();
}

void PathArea::updatePath()
{
    QtPolygonRounder<ManhattanDistance> rounder;
    mPath = rounder(buildPolygon(contentsRect()), mRadius);
}

QPolygon PathArea::buildPolygon(const QRect& boundingRect)
{
    if (mEdges == Qt::Edges(0))
        return QPolygon(boundingRect);

    const int h = mArrowSize.height();
    const int w = mArrowSize.width();

    QRect rect = boundingRect;
    for (int i = Qt::TopEdge; i <= Qt::BottomEdge; i <<= 1)
    {
        if (!(mEdges & i))
            continue;

        switch(i)
        {
        case Qt::TopEdge:
            rect.adjust(0, h, 0, 0);
            break;
        case Qt::LeftEdge:
            rect.adjust(h, 0, 0, 0);
            break;
        case Qt::RightEdge:
            rect.adjust(0, 0, -h, 0);
            break;
        case Qt::BottomEdge:
            rect.adjust(0, 0, 0, -h);
            break;
        }
    }

    const QPoint center = boundingRect.center();

    QPolygon polygon;
    polygon.reserve(polygonVertices(mEdges));

    // process edges from top in clockwise manner
    if ((mEdges & Qt::TopEdge))
    {
        polygon.push_back( { center.x() - w/2, rect.top() } );
        polygon.push_back( { center.x(), rect.top() - h });
        polygon.push_back( { center.x() + w/2, rect.top() });
    }
    polygon.push_back(rect.topRight());

    if ((mEdges & Qt::RightEdge))
    {
        polygon.push_back( { rect.right(), center.y() - w/2 } );
        polygon.push_back( { rect.right() + h, center.y() });
        polygon.push_back( { rect.right(), center.y() + w/2 });
    }
    polygon.push_back(rect.bottomRight());

    if ((mEdges & Qt::BottomEdge))
    {
        polygon.push_back( { center.x() + w/2, rect.bottom() } );
        polygon.push_back( { center.x(), rect.bottom() + h });
        polygon.push_back( { center.x() - w/2, rect.bottom() });
    }
    polygon.push_back(rect.bottomLeft());

    if ((mEdges & Qt::LeftEdge))
    {
        polygon.push_back( { rect.left(), center.y() + w/2 } );
        polygon.push_back( { rect.left() - h, center.y()  });
        polygon.push_back( { rect.left(), center.y() - w/2 });
    }
    polygon.push_back(rect.topLeft());

    return polygon;
}

int PathArea::polygonVertices(Qt::Edges edges) const
{
    int n = 3;
    for (int i = Qt::TopEdge; i <= Qt::BottomEdge; i <<= 1)
    {
        if (mEdges & i)
            n += 3;
    }
    return n;
}
