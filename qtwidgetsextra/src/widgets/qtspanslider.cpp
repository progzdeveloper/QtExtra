#include "qtspanslider.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionSlider>

class QtSpanSliderPrivate
{
public:
    QtSpanSlider* q_ptr;
    int lower;
    int upper;
    int lowerPos;
    int upperPos;
    int offset;
    int position;
    QtSpanSlider::SpanHandle lastPressed;
    QtSpanSlider::SpanHandle mainControl;
    QStyle::SubControl lowerPressed;
    QStyle::SubControl upperPressed;
    QtSpanSlider::HandleMovementMode movement;

    bool firstMovement : 1;
    bool blockTracking : 1;

    QtSpanSliderPrivate(QtSpanSlider* q);

    void initStyleOption(QStyleOptionSlider* option, QtSpanSlider::SpanHandle handle = QtSpanSlider::UpperHandle) const;
    int pick(const QPoint& pt) const;
    int pixelPosToRangeValue(int pos) const;
    void handleMousePress(const QPoint& pos, QStyle::SubControl& control, int value, QtSpanSlider::SpanHandle handle);
    void drawHandle(QStylePainter* painter, QtSpanSlider::SpanHandle handle) const;
    void setupPainter(QPainter* painter, Qt::Orientation orientation, qreal x1, qreal y1, qreal x2, qreal y2) const;
    void drawSpan(QStylePainter* painter, const QRect& rect) const;
    void triggerAction(QAbstractSlider::SliderAction action, bool main);
    void swapControls();

    void updateRange(int min, int max);
    void movePressedHandle();
};


QtSpanSliderPrivate::QtSpanSliderPrivate(QtSpanSlider* q) :
    q_ptr(q),
    lower(0),
    upper(0),
    lowerPos(0),
    upperPos(0),
    offset(0),
    position(0),
    lastPressed(QtSpanSlider::NoHandle),
    mainControl(QtSpanSlider::LowerHandle),
    lowerPressed(QStyle::SC_None),
    upperPressed(QStyle::SC_None),
    movement(QtSpanSlider::FreeMovement),
    firstMovement(false),
    blockTracking(false)
{
}

void QtSpanSliderPrivate::initStyleOption(QStyleOptionSlider* option, QtSpanSlider::SpanHandle handle) const
{
    q_ptr->initStyleOption(option);
    option->sliderPosition = (handle == QtSpanSlider::LowerHandle ? lowerPos : upperPos);
    option->sliderValue = (handle == QtSpanSlider::LowerHandle ? lower : upper);
}

int QtSpanSliderPrivate::pick(const QPoint &pt) const
{
    return q_ptr->orientation() == Qt::Horizontal ? pt.x() : pt.y();
}

int QtSpanSliderPrivate::pixelPosToRangeValue(int pos) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    int sliderMin = 0;
    int sliderMax = 0;
    int sliderLength = 0;
    const QRect gr = q_ptr->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, q_ptr);
    const QRect sr = q_ptr->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, q_ptr);
    if (q_ptr->orientation() == Qt::Horizontal)
    {
        sliderLength = sr.width();
        sliderMin = gr.x();
        sliderMax = gr.right() - sliderLength + 1;
    }
    else
    {
        sliderLength = sr.height();
        sliderMin = gr.y();
        sliderMax = gr.bottom() - sliderLength + 1;
    }
    return QStyle::sliderValueFromPosition(q_ptr->minimum(), q_ptr->maximum(), pos - sliderMin,
                                           sliderMax - sliderMin, opt.upsideDown);
}

void QtSpanSliderPrivate::handleMousePress(const QPoint& pos, QStyle::SubControl& control, int value, QtSpanSlider::SpanHandle handle)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt, handle);
    const QStyle::SubControl oldControl = control;
    control = q_ptr->style()->hitTestComplexControl(QStyle::CC_Slider, &opt, pos, q_ptr);
    const QRect sr = q_ptr->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, q_ptr);
    if (control == QStyle::SC_SliderHandle)
    {
        position = value;
        offset = pick(pos - sr.topLeft());
        lastPressed = handle;
        q_ptr->setSliderDown(true);
        emit q_ptr->sliderPressed(handle);
    }
    if (control != oldControl)
        q_ptr->update(sr);
}

void QtSpanSliderPrivate::setupPainter(QPainter* painter, Qt::Orientation orientation, qreal x1, qreal y1, qreal x2, qreal y2) const
{
    QColor highlight = q_ptr->palette().color(QPalette::Highlight);
    QLinearGradient gradient(x1, y1, x2, y2);
    gradient.setColorAt(0, highlight.dark(120));
    gradient.setColorAt(1, highlight.light(108));
    painter->setBrush(gradient);

    if (orientation == Qt::Horizontal)
        painter->setPen(QPen(highlight.dark(130), 0));
    else
        painter->setPen(QPen(highlight.dark(150), 0));
}

void QtSpanSliderPrivate::drawSpan(QStylePainter* painter, const QRect& rect) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    // area
    QRect groove = q_ptr->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, q_ptr);
    if (opt.orientation == Qt::Horizontal)
        groove.adjust(0, 0, -1, 0);
    else
        groove.adjust(0, 0, 0, -1);

    // pen & brush
    painter->setPen(QPen(q_ptr->palette().color(QPalette::Dark).light(110), 0));
    if (opt.orientation == Qt::Horizontal)
        setupPainter(painter, opt.orientation, groove.center().x(), groove.top(), groove.center().x(), groove.bottom());
    else
        setupPainter(painter, opt.orientation, groove.left(), groove.center().y(), groove.right(), groove.center().y());

    // draw groove
    painter->drawRect(rect.intersected(groove));
}

void QtSpanSliderPrivate::drawHandle(QStylePainter* painter, QtSpanSlider::SpanHandle handle) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt, handle);
    opt.subControls = QStyle::SC_SliderHandle;
    QStyle::SubControl pressed = (handle == QtSpanSlider::LowerHandle ? lowerPressed : upperPressed);
    if (pressed == QStyle::SC_SliderHandle)
    {
        opt.activeSubControls = pressed;
        opt.state |= QStyle::State_Sunken;
    }
    painter->drawComplexControl(QStyle::CC_Slider, opt);
}

void QtSpanSliderPrivate::triggerAction(QAbstractSlider::SliderAction action, bool main)
{
    int value = 0;
    bool no = false;
    bool up = false;
    const int min = q_ptr->minimum();
    const int max = q_ptr->maximum();
    const QtSpanSlider::SpanHandle altControl = (mainControl == QtSpanSlider::LowerHandle ? QtSpanSlider::UpperHandle : QtSpanSlider::LowerHandle);

    blockTracking = true;

    switch (action)
    {
    case QAbstractSlider::SliderSingleStepAdd:
        if ((main && mainControl == QtSpanSlider::UpperHandle) || (!main && altControl == QtSpanSlider::UpperHandle))
        {
            value = qBound(min, upper + q_ptr->singleStep(), max);
            up = true;
            break;
        }
        value = qBound(min, lower + q_ptr->singleStep(), max);
        break;
    case QAbstractSlider::SliderSingleStepSub:
        if ((main && mainControl == QtSpanSlider::UpperHandle) || (!main && altControl == QtSpanSlider::UpperHandle))
        {
            value = qBound(min, upper - q_ptr->singleStep(), max);
            up = true;
            break;
        }
        value = qBound(min, lower - q_ptr->singleStep(), max);
        break;
    case QAbstractSlider::SliderToMinimum:
        value = min;
        if ((main && mainControl == QtSpanSlider::UpperHandle) || (!main && altControl == QtSpanSlider::UpperHandle))
            up = true;
        break;
    case QAbstractSlider::SliderToMaximum:
        value = max;
        if ((main && mainControl == QtSpanSlider::UpperHandle) || (!main && altControl == QtSpanSlider::UpperHandle))
            up = true;
        break;
    case QAbstractSlider::SliderMove:
        if ((main && mainControl == QtSpanSlider::UpperHandle) || (!main && altControl == QtSpanSlider::UpperHandle))
            up = true;
    case QAbstractSlider::SliderNoAction:
        no = true;
        break;
    default:
        qWarning("QxtSpanSliderPrivate::triggerAction: Unknown action");
        break;
    }

    if (!no && !up)
    {
        if (movement == QtSpanSlider::NoCrossing)
            value = qMin(value, upper);
        else if (movement == QtSpanSlider::NoOverlapping)
            value = qMin(value, upper - 1);

        if (movement == QtSpanSlider::FreeMovement && value > upper)
        {
            swapControls();
            q_ptr->setUpperPosition(value);
        }
        else
        {
            q_ptr->setLowerPosition(value);
        }
    }
    else if (!no)
    {
        if (movement == QtSpanSlider::NoCrossing)
            value = qMax(value, lower);
        else if (movement == QtSpanSlider::NoOverlapping)
            value = qMax(value, lower + 1);

        if (movement == QtSpanSlider::FreeMovement && value < lower)
        {
            swapControls();
            q_ptr->setLowerPosition(value);
        }
        else
        {
            q_ptr->setUpperPosition(value);
        }
    }

    blockTracking = false;
    q_ptr->setLowerValue(lowerPos);
    q_ptr->setUpperValue(upperPos);
}

void QtSpanSliderPrivate::swapControls()
{
    qSwap(lower, upper);
    qSwap(lowerPressed, upperPressed);
    lastPressed = (lastPressed == QtSpanSlider::LowerHandle ? QtSpanSlider::UpperHandle : QtSpanSlider::LowerHandle);
    mainControl = (mainControl == QtSpanSlider::LowerHandle ? QtSpanSlider::UpperHandle : QtSpanSlider::LowerHandle);
}

void QtSpanSliderPrivate::updateRange(int min, int max)
{
    Q_UNUSED(min);
    Q_UNUSED(max);
    // setSpan() takes care of keeping span in range
    q_ptr->setSpan(lower, upper);
}

void QtSpanSliderPrivate::movePressedHandle()
{
    switch (lastPressed)
    {
        case QtSpanSlider::LowerHandle:
            if (lowerPos != lower)
            {
                bool main = (mainControl == QtSpanSlider::LowerHandle);
                triggerAction(QAbstractSlider::SliderMove, main);
            }
            break;
        case QtSpanSlider::UpperHandle:
            if (upperPos != upper)
            {
                bool main = (mainControl == QtSpanSlider::UpperHandle);
                triggerAction(QAbstractSlider::SliderMove, main);
            }
            break;
        default:
            break;
    }
}



/*!
    \class QtSpanSlider
    \brief The QtSpanSlider widget is a QSlider with two handles.
    QtSpanSlider is a slider with two handles. QtSpanSlider is
    handy for letting user to choose an span between min/max.
    The span color is calculated based on QPalette::Highlight.
    The keys are bound according to the following table:
    \table
    \header \o Orientation    \o Key           \o Handle
    \row    \o Qt::Horizontal \o Qt::Key_Left  \o lower
    \row    \o Qt::Horizontal \o Qt::Key_Right \o lower
    \row    \o Qt::Horizontal \o Qt::Key_Up    \o upper
    \row    \o Qt::Horizontal \o Qt::Key_Down  \o upper
    \row    \o Qt::Vertical   \o Qt::Key_Up    \o lower
    \row    \o Qt::Vertical   \o Qt::Key_Down  \o lower
    \row    \o Qt::Vertical   \o Qt::Key_Left  \o upper
    \row    \o Qt::Vertical   \o Qt::Key_Right \o upper
    \endtable
    Keys are bound by the time the slider is created. A key is bound
    to same handle for the lifetime of the slider. So even if the handle
    representation might change from lower to upper, the same key binding
    remains.

    \bold {Note:} QtSpanSlider inherits QSlider for implementation specific
    reasons. Adjusting any single handle specific properties like
    \list
    \o QAbstractSlider::sliderPosition
    \o QAbstractSlider::value
    \endlist
    has no effect. However, all slider specific properties like
    \list
    \o QAbstractSlider::invertedAppearance
    \o QAbstractSlider::invertedControls
    \o QAbstractSlider::minimum
    \o QAbstractSlider::maximum
    \o QAbstractSlider::orientation
    \o QAbstractSlider::pageStep
    \o QAbstractSlider::singleStep
    \o QSlider::tickInterval
    \o QSlider::tickPosition
    \endlist
    are taken into consideration.
 */

/*!
    \enum QtSpanSlider::HandleMovementMode
    This enum describes the available handle movement modes.
    \value FreeMovement The handles can be moved freely.
    \value NoCrossing The handles cannot cross, but they can still overlap each other. The lower and upper values can be the same.
    \value NoOverlapping The handles cannot overlap each other. The lower and upper values cannot be the same.
 */

/*!
    \enum QtSpanSlider::SpanHandle
    This enum describes the available span handles.
    \omitvalue NoHandle \omit Internal only (for now). \endomit
    \value LowerHandle The lower boundary handle.
    \value UpperHandle The upper boundary handle.
 */

/*!
    \fn QtSpanSlider::lowerValueChanged(int lower)
    This signal is emitted whenever the \a lower value has changed.
 */

/*!
    \fn QtSpanSlider::upperValueChanged(int upper)
    This signal is emitted whenever the \a upper value has changed.
 */

/*!
    \fn QtSpanSlider::spanChanged(int lower, int upper)
    This signal is emitted whenever both the \a lower and the \a upper
    values have changed ie. the span has changed.
 */

/*!
    \fn QtSpanSlider::lowerPositionChanged(int lower)
    This signal is emitted whenever the \a lower position has changed.
 */

/*!
    \fn QtSpanSlider::upperPositionChanged(int upper)
    This signal is emitted whenever the \a upper position has changed.
 */

/*!
    \fn QtSpanSlider::sliderPressed(SpanHandle handle)
    This signal is emitted whenever the \a handle has been pressed.
 */






/*!
 * \brief QtSpanSlider::QtSpanSlider
 * \param parent
 */
QtSpanSlider::QtSpanSlider(QWidget *parent) :
    QSlider(parent),
    d_ptr(new QtSpanSliderPrivate(this))
{
    connect(this, SIGNAL(rangeChanged(int, int)), this, SLOT(updateRange(int, int)));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(movePressedHandle()));
}

/*!
 * \brief QtSpanSlider::QtSpanSlider
 * \param orientation
 * \param parent
 */
QtSpanSlider::QtSpanSlider(Qt::Orientation orientation, QWidget *parent) :
    QSlider(orientation, parent),
    d_ptr(new QtSpanSliderPrivate(this))
{
    connect(this, SIGNAL(rangeChanged(int, int)), this, SLOT(updateRange(int, int)));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(movePressedHandle()));
}

/*!
 * \brief QtSpanSlider::~QtSpanSlider
 *
 * destroy QtSpanSlider
 */
QtSpanSlider::~QtSpanSlider()
{
}

/*!
    \property QtSpanSlider::handleMovementMode
    \brief the handle movement mode
 */
QtSpanSlider::HandleMovementMode QtSpanSlider::handleMovementMode() const
{
    Q_D(const QtSpanSlider);
    return d->movement;
}

void QtSpanSlider::setHandleMovementMode(QtSpanSlider::HandleMovementMode mode)
{
    Q_D(QtSpanSlider);
    d->movement = mode;
}

/*!
    \property QtSpanSlider::lowerValue
    \brief the lower value of the span
 */
int QtSpanSlider::lowerValue() const
{
    Q_D(const QtSpanSlider);
    return qMin(d->lower, d->upper);
}

void QtSpanSlider::setLowerValue(int lower)
{
    Q_D(QtSpanSlider);
    setSpan(lower, d->upper);
}

/*!
    \property QtSpanSlider::upperValue
    \brief the upper value of the span
 */
int QtSpanSlider::upperValue() const
{
    Q_D(const QtSpanSlider);
    return qMax(d->lower, d->upper);
}

void QtSpanSlider::setUpperValue(int upper)
{
    Q_D(QtSpanSlider);
    setSpan(d->lower, upper);
}

/*!
    Sets the span from \a lower to \a upper.
 */
void QtSpanSlider::setSpan(int lower, int upper)
{
    Q_D(QtSpanSlider);
    const int low = qBound(minimum(), qMin(lower, upper), maximum());
    const int upp = qBound(minimum(), qMax(lower, upper), maximum());
    if (low != d->lower || upp != d->upper)
    {
        if (low != d->lower)
        {
            d->lower = low;
            d->lowerPos = low;
            emit lowerValueChanged(low);
        }
        if (upp != d->upper)
        {
            d->upper = upp;
            d->upperPos = upp;
            emit upperValueChanged(upp);
        }
        emit spanChanged(d->lower, d->upper);
        update();
    }
}

/*!
    \property QtSpanSlider::lowerPosition
    \brief the lower position of the span
 */
int QtSpanSlider::lowerPosition() const
{
    Q_D(const QtSpanSlider);
    return d->lowerPos;
}

void QtSpanSlider::setLowerPosition(int lower)
{
    Q_D(QtSpanSlider);
    if (d->lowerPos != lower)
    {
        d->lowerPos = lower;
        if (!hasTracking())
            update();
        if (isSliderDown())
            emit lowerPositionChanged(lower);
        if (hasTracking() && !d->blockTracking)
        {
            bool main = (d->mainControl == QtSpanSlider::LowerHandle);
            d->triggerAction(SliderMove, main);
        }
    }
}

/*!
    \property QtSpanSlider::upperPosition
    \brief the upper position of the span
 */
int QtSpanSlider::upperPosition() const
{
    Q_D(const QtSpanSlider);
    return d->upperPos;
}

void QtSpanSlider::setUpperPosition(int upper)
{
    Q_D(QtSpanSlider);
    if (d->upperPos != upper)
    {
        d->upperPos = upper;
        if (!hasTracking())
            update();
        if (isSliderDown())
            emit upperPositionChanged(upper);
        if (hasTracking() && !d->blockTracking)
        {
            bool main = (d->mainControl == QtSpanSlider::UpperHandle);
            d->triggerAction(SliderMove, main);
        }
    }
}

/*!
    \reimp
 */
void QtSpanSlider::keyPressEvent(QKeyEvent* event)
{
    Q_D(QtSpanSlider);
    QSlider::keyPressEvent(event);

    bool main = true;
    SliderAction action = SliderNoAction;
    switch (event->key())
    {
    case Qt::Key_Left:
        main   = (orientation() == Qt::Horizontal);
        action = !invertedAppearance() ? SliderSingleStepSub : SliderSingleStepAdd;
        break;
    case Qt::Key_Right:
        main   = (orientation() == Qt::Horizontal);
        action = !invertedAppearance() ? SliderSingleStepAdd : SliderSingleStepSub;
        break;
    case Qt::Key_Up:
        main   = (orientation() == Qt::Vertical);
        action = invertedControls() ? SliderSingleStepSub : SliderSingleStepAdd;
        break;
    case Qt::Key_Down:
        main   = (orientation() == Qt::Vertical);
        action = invertedControls() ? SliderSingleStepAdd : SliderSingleStepSub;
        break;
    case Qt::Key_Home:
        main   = (d->mainControl == QtSpanSlider::LowerHandle);
        action = SliderToMinimum;
        break;
    case Qt::Key_End:
        main   = (d->mainControl == QtSpanSlider::UpperHandle);
        action = SliderToMaximum;
        break;
    default:
        event->ignore();
        break;
    }

    if (action)
        d->triggerAction(action, main);
}

/*!
    \reimp
 */
void QtSpanSlider::mousePressEvent(QMouseEvent* event)
{
    Q_D(QtSpanSlider);
    if (minimum() == maximum() || (event->buttons() ^ event->button()))
    {
        event->ignore();
        return;
    }

    d->handleMousePress(event->pos(), d->upperPressed, d->upper, QtSpanSlider::UpperHandle);
    if (d->upperPressed != QStyle::SC_SliderHandle)
        d->handleMousePress(event->pos(), d->lowerPressed, d->lower, QtSpanSlider::LowerHandle);

    d->firstMovement = true;
    event->accept();
}

/*!
    \reimp
 */
void QtSpanSlider::mouseMoveEvent(QMouseEvent* event)
{
    Q_D(QtSpanSlider);
    if (d->lowerPressed != QStyle::SC_SliderHandle && d->upperPressed != QStyle::SC_SliderHandle)
    {
        event->ignore();
        return;
    }

    QStyleOptionSlider opt;
    d->initStyleOption(&opt);
    const int m = style()->pixelMetric(QStyle::PM_MaximumDragDistance, &opt, this);
    int newPosition = d->pixelPosToRangeValue(d->pick(event->pos()) - d->offset);
    if (m >= 0)
    {
        const QRect r = rect().adjusted(-m, -m, m, m);
        if (!r.contains(event->pos()))
        {
            newPosition = d->position;
        }
    }

    // pick the preferred handle on the first movement
    if (d->firstMovement)
    {
        if (d->lower == d->upper)
        {
            if (newPosition < lowerValue())
            {
                d->swapControls();
                d->firstMovement = false;
            }
        }
        else
        {
            d->firstMovement = false;
        }
    }

    if (d->lowerPressed == QStyle::SC_SliderHandle)
    {
        if (d->movement == NoCrossing)
            newPosition = qMin(newPosition, upperValue());
        else if (d->movement == NoOverlapping)
            newPosition = qMin(newPosition, upperValue() - 1);

        if (d->movement == FreeMovement && newPosition > d->upper)
        {
            d->swapControls();
            setUpperPosition(newPosition);
        }
        else
        {
            setLowerPosition(newPosition);
        }
    }
    else if (d->upperPressed == QStyle::SC_SliderHandle)
    {
        if (d->movement == NoCrossing)
            newPosition = qMax(newPosition, lowerValue());
        else if (d->movement == NoOverlapping)
            newPosition = qMax(newPosition, lowerValue() + 1);

        if (d->movement == FreeMovement && newPosition < d->lower)
        {
            d->swapControls();
            setLowerPosition(newPosition);
        }
        else
        {
            setUpperPosition(newPosition);
        }
    }
    event->accept();
}

/*!
    \reimp
 */
void QtSpanSlider::mouseReleaseEvent(QMouseEvent* event)
{
    Q_D(QtSpanSlider);
    QSlider::mouseReleaseEvent(event);
    setSliderDown(false);
    d->lowerPressed = QStyle::SC_None;
    d->upperPressed = QStyle::SC_None;
    update();
}

/*!
    \reimp
 */
void QtSpanSlider::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    Q_D(QtSpanSlider);

    QStylePainter painter(this);

    // groove & ticks
    QStyleOptionSlider opt;
    d->initStyleOption(&opt);
    opt.sliderValue = 0;
    opt.sliderPosition = 0;
    opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderTickmarks;
    painter.drawComplexControl(QStyle::CC_Slider, opt);

    // handle rects
    opt.sliderPosition = d->lowerPos;
    const QRect lr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    const int lrv  = d->pick(lr.center());
    opt.sliderPosition = d->upperPos;
    const QRect ur = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    const int urv  = d->pick(ur.center());

    // span
    const int minv = qMin(lrv, urv);
    const int maxv = qMax(lrv, urv);
    const QPoint c = QRect(lr.center(), ur.center()).center();
    QRect spanRect;
    if (orientation() == Qt::Horizontal)
        spanRect = QRect(QPoint(minv, c.y() - 2), QPoint(maxv, c.y() + 1));
    else
        spanRect = QRect(QPoint(c.x() - 2, minv), QPoint(c.x() + 1, maxv));
    d->drawSpan(&painter, spanRect);

    // handles
    switch (d->lastPressed)
    {
    case QtSpanSlider::LowerHandle:
        d->drawHandle(&painter, QtSpanSlider::UpperHandle);
        d->drawHandle(&painter, QtSpanSlider::LowerHandle);
        break;
    case QtSpanSlider::UpperHandle:
    default:
        d->drawHandle(&painter, QtSpanSlider::LowerHandle);
        d->drawHandle(&painter, QtSpanSlider::UpperHandle);
        break;
    }
}

void QtSpanSlider::updateRange(int minimum, int maximum)
{
    Q_D(QtSpanSlider);
    d->updateRange(minimum, maximum);
}

void QtSpanSlider::movePressedHandle()
{
    Q_D(QtSpanSlider);
    d->movePressedHandle();
}
