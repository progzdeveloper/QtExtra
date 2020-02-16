#include "qtprogresseffect.h"
#include <QPointer>
#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionProgressBar>

class QtProgressEffectPrivate
{
public:
    QString labelText;
    QPointer<QWidget> widget;
    QStyle* style;
    int timerId;
    int minimum;
    int maximum;
    int progress;
    qreal opacity;

    QtProgressEffectPrivate(QWidget* w) :
        widget(w), minimum(0), maximum(0), progress(0), opacity(1.0) {}
};

QtProgressEffect::QtProgressEffect(QObject *parent) :
    QGraphicsEffect(parent), d_ptr(new QtProgressEffectPrivate(Q_NULLPTR))
{
    Q_D(QtProgressEffect);
    d->timerId = startTimer(40);
}

QtProgressEffect::QtProgressEffect(QWidget *widget, QObject *parent) :
    QGraphicsEffect(parent), d_ptr(new QtProgressEffectPrivate(widget))
{
    Q_D(QtProgressEffect);
    d->timerId = startTimer(40);
    if (widget) {
        widget->setGraphicsEffect(this);
        d->style = (d->widget ? d->widget->style() : QApplication::style());
    }
}

QtProgressEffect::~QtProgressEffect()
{
    Q_D(QtProgressEffect);
    killTimer(d->timerId);
}

void QtProgressEffect::setOpacity(qreal opacity)
{
    Q_D(QtProgressEffect);
    if (d->opacity != opacity) {
        d->opacity = opacity;
        Q_EMIT opacityChanged(d->opacity);
        update();
    }
}

qreal QtProgressEffect::opacity() const
{
    Q_D(const QtProgressEffect);
    return d->opacity ;
}

void QtProgressEffect::setMinimum(int minimum)
{
    Q_D(QtProgressEffect);
    if (d->minimum != minimum) {
        d->minimum = minimum;
        update();
    }
}

int QtProgressEffect::minimum() const
{
    Q_D(const QtProgressEffect);
    return d->minimum;
}

void QtProgressEffect::setMaximum(int maximum)
{
    Q_D(QtProgressEffect);
    if (d->maximum != maximum) {
        d->maximum = maximum;
        update();
    }
}

int QtProgressEffect::maximum() const
{
    Q_D(const QtProgressEffect);
    return d->maximum;
}

void QtProgressEffect::setRange(int minimum, int maximum)
{
    Q_D(QtProgressEffect);
    if (d->minimum != minimum || d->maximum != maximum) {
        d->minimum = minimum;
        d->maximum = maximum;
        update();
    }
}

void QtProgressEffect::setLabelText(const QString &text)
{
    Q_D(QtProgressEffect);
    if (d->labelText != text) {
        d->labelText = text;
        update();
    }
}

QString QtProgressEffect::labelText() const
{
    Q_D(const QtProgressEffect);
    return d->labelText;
}

void QtProgressEffect::setProgress(int value)
{
    Q_D(QtProgressEffect);
    if (d->progress != value) {
        d->progress = value;
        update();
    }
}

int QtProgressEffect::progress() const
{
    Q_D(const QtProgressEffect);
    return d->progress;
}

void QtProgressEffect::draw(QPainter *painter)
{
    Q_D(QtProgressEffect);

    /*painter->save();
    drawSource(painter);
    painter->restore();*/

    painter->save();
    painter->setOpacity(d->opacity);
    painter->fillRect(boundingRect(), Qt::darkGray);

    QRect rect = boundingRect().toRect();
    int w = qMin(rect.width() > 16 ? rect.width() - 16 : rect.width(), 256);
    int h = qMin(rect.height() > 4 ? rect.height() - 4 : rect.height(), 24);

    QRect barRect(rect.center().x() - w / 2, rect.height() / 2 - h / 2, w, h);

    QStyleOptionProgressBar progressBarOption;
    progressBarOption.rect = barRect;
    progressBarOption.minimum = d->minimum;
    progressBarOption.maximum = d->maximum;
    progressBarOption.progress = d->progress;
    if (!d->labelText.isEmpty())
        progressBarOption.text += d->labelText;
    if (d->minimum != d->maximum)
        progressBarOption.text += ' ' + (QString::number(d->progress) + " %");
    progressBarOption.textVisible = true;

    progressBarOption.textAlignment = Qt::AlignCenter;
    progressBarOption.state = QStyle::State_Enabled|QStyle::State_On|QStyle::State_Active;

    painter->setOpacity(1.0);
    d->style->drawControl(QStyle::CE_ProgressBar,
                       &progressBarOption, painter);
    painter->restore();
}

void QtProgressEffect::timerEvent(QTimerEvent*)
{
    Q_D(QtProgressEffect);
    if (d->minimum == d->maximum) {
        update();
        qApp->processEvents();
    }
}
