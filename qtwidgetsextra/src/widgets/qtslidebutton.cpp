#include "qtslidebutton_p.h"

#include <QPen>
#include <QBrush>
#include <QPalette>
#include <QPainter>

#include <QPropertyAnimation>

#include <QPaintEvent>
#include <QResizeEvent>

QtSlideButtonPrivate::QtSlideButtonPrivate(QtSlideButton *q) :
    q_ptr(q), pos(0.0), animation(new QPropertyAnimation(this))
{
    animation->setTargetObject(this);
    animation->setPropertyName("position");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setDuration(300);
    animation->setEasingCurve(QEasingCurve::InOutExpo);
    gradient.setSpread(QGradient::PadSpread);
}

QtSlideButtonPrivate::~QtSlideButtonPrivate()
{
    delete animation;
}

void QtSlideButtonPrivate::setPosition(qreal value)
{
    pos = value;
    q_ptr->repaint();
}

qreal QtSlideButtonPrivate::position() const
{
    return pos;
}

void QtSlideButtonPrivate::updateSliderRect(const QSize& size)
{
    sliderShape.setWidth(size.width() / 2.0);
    sliderShape.setHeight(size.height()-1.0);
}

void QtSlideButtonPrivate::drawSlider(QPainter *painter)
{
    const qreal margin = 4;

    QRectF r = q_ptr->rect().adjusted(0, 0, -1, -1);
    qreal dx = (r.width() - sliderShape.width()) * pos;
    QRectF sliderRect = sliderShape.translated(dx, 0);

    painter->setPen(Qt::NoPen);

    // basic settings
    QColor shadow = q_ptr->palette().color(QPalette::Dark);
    QColor light = q_ptr->palette().color(QPalette::Light);
    QColor button =  q_ptr->palette().color(QPalette::Button);

    QColor internalShadow = q_ptr->isChecked() ? q_ptr->palette().color(QPalette::Highlight).darker(180) : shadow.darker(140);
    QColor internalLight = q_ptr->isChecked() ? q_ptr->palette().color(QPalette::Highlight) : light.darker(140);

    // draw background
    // draw background outer
    gradient.setColorAt(0, light.darker(140));
    gradient.setColorAt(1, light);
    gradient.setStart(0, r.height());
    gradient.setFinalStop(0, 0);
    painter->setBrush(gradient);
    painter->drawRoundedRect(r, 12, 12);

    // draw background inner
    gradient.setColorAt(0, internalShadow);
    gradient.setColorAt(1, internalLight);
    gradient.setStart(0, 0);
    gradient.setFinalStop(0, r.height());
    painter->setBrush(gradient);
    painter->drawRoundedRect(r.adjusted(margin, margin, -margin, -margin), 12, 12);

    // draw slider
    gradient.setColorAt(0, button.darker(120));
    gradient.setColorAt(1, button);

    // draw outer slider
    gradient.setStart(0, r.height());
    gradient.setFinalStop(0, 0);
    painter->setBrush(gradient);
    painter->drawRoundedRect(sliderRect.adjusted(margin-1, margin-1, -margin+1, -margin+1), 12, 12);

    // draw inner slider
    gradient.setStart(0, 0);
    gradient.setFinalStop(0, r.height());
    painter->setBrush(gradient);
    painter->drawRoundedRect(sliderRect.adjusted(2.1 * margin, 2.1 *  margin, -2.1 * margin, -2.1 * margin), 5, 5);

    // draw text
    if (animation->state() == QPropertyAnimation::Running)
        return; // don't draw any text while animation is running

    QFont font = q_ptr->font();
    gradient.setColorAt(0, light);
    gradient.setColorAt(1, shadow);
    gradient.setStart(0, r.height()/2.0 + font.pointSizeF());
    gradient.setFinalStop(0, r.height()/2.0 - font.pointSizeF());
    painter->setFont(font);
    painter->setPen(QPen(QBrush(gradient), 0));
    if (q_ptr->isChecked()) {
        painter->drawText(0, 0, r.width()/2, r.height()-1, Qt::AlignCenter, tr("ON"));
    } else {
        painter->drawText(r.width()/2, 0, r.width()/2, r.height()-1, Qt::AlignCenter, tr("OFF"));
    }
}

void QtSlideButtonPrivate::animate(bool checked)
{
    animation->setDirection(checked ? QPropertyAnimation::Forward : QPropertyAnimation::Backward);
    animation->start();
}



QtSlideButton::QtSlideButton(QWidget *parent /*= 0*/) :
    QAbstractButton(parent), d_ptr(new QtSlideButtonPrivate(this))
{
    // animate changing button state
    connect(this, SIGNAL(clicked(bool)), d_ptr.data(), SLOT(animate(bool)));
    // tigger update on animation finished
    connect(d_ptr->animation, SIGNAL(finished()), this, SLOT(update()));
    setCheckable(true);
}

QtSlideButton::~QtSlideButton()
{
}

QSize QtSlideButton::sizeHint() const
{
    return QSize(46, 32);
}

bool QtSlideButton::hitButton(const QPoint& p) const
{
    return rect().contains(p);
}

void QtSlideButton::paintEvent(QPaintEvent *)
{
    Q_D(QtSlideButton);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    d->drawSlider(&painter);
}

void QtSlideButton::resizeEvent(QResizeEvent *e)
{
    Q_D(QtSlideButton);
    d->updateSliderRect(e->size());
    repaint();
}





