#include "qtswitchbutton.h"
#include <QCoreApplication>
#include <QEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QBrush>
#include <QPalette>

class QtSwitchButtonPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtSwitchButtonPrivate)
public:
    QRect rect;
    int marging;
    QtSwitchButtonPrivate(QtSwitchButton* w);
    void render(QPainter* painter, QtSwitchButton* w);
};

QtSwitchButtonPrivate::QtSwitchButtonPrivate(QtSwitchButton *w) :
    marging(6)
{
    rect = w->rect().adjusted(marging,  marging + w->fontMetrics().height(),
                             -marging, -marging);
}

void QtSwitchButtonPrivate::render(QPainter *painter, QtSwitchButton *w)
{
    const QPalette& palette = w->palette();
    double radius = 5.5;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(palette.dark());
    painter->drawRoundedRect(w->rect(), radius, radius);

    painter->setPen(palette.light().color());
    painter->drawText(w->rect().adjusted(0, marging / 2, 0, 0), w->isChecked() ? tr("ON") : tr("OFF"), QTextOption(Qt::AlignHCenter));

    painter->setPen(Qt::NoPen);


    painter->setBrush(w->isChecked() ? palette.midlight() : palette.button().color().lighter(90));
    painter->drawRect(rect.adjusted(0, 0, 0, -(rect.height() / 2)));

    painter->setBrush(w->isChecked() ? palette.button().color().lighter(90) : palette.midlight());
    painter->drawRect(rect.adjusted(0, rect.height() / 2, 0, 0));

    if (w->isChecked()) {
        painter->setBrush(palette.mid());
        painter->drawRect(rect.adjusted(0, rect.height() - rect.height() / 20, 0, 0));
    } else {
        painter->setBrush(palette.mid());
        painter->drawRect(QRect(rect.x(), rect.y(), rect.width(), rect.height() / 20));
    }
}


QtSwitchButton::QtSwitchButton(QWidget *parent) :
    QAbstractButton(parent), d_ptr(new QtSwitchButtonPrivate(this))
{
    setCheckable(true);
    setChecked(true);
}

QtSwitchButton::~QtSwitchButton()
{

}

void QtSwitchButton::paintEvent(QPaintEvent *)
{
    Q_D(QtSwitchButton);
    QPainter painter(this);
    d->render(&painter, this);
}

bool QtSwitchButton::hitButton(const QPoint &pos) const
{
    Q_D(const QtSwitchButton);
    if (isChecked()) {
        return d->rect.adjusted(0, d->rect.height() / 2, 0, 0).contains(pos);
    } else {
        return d->rect.adjusted(0, 0, 0, -(d->rect.height() / 2)).contains(pos);
    }
}


void QtSwitchButton::resizeEvent(QResizeEvent *event)
{
    Q_D(QtSwitchButton);
    d->rect = QRect(QPoint(0, 0), event->size()).adjusted(d->marging, d->marging + fontMetrics().height(),
                                                       -d->marging, -d->marging);
    QAbstractButton::resizeEvent(event);
}


