#include "demowidget.h"
#include <QFormLayout>
#include <QBoxLayout>

#include <QLabel>

#include <QtGlobalExtra>

#include <QtLongSpinBox>
#include <QtTextSpinBox>
#include <QtSpanSlider>

#include <QtSlideButton>

DemoWidget::DemoWidget(QWidget *parent)
    : QWidget(parent)
{
    //layout = new QFormLayout(this);
    layout = new QVBoxLayout(this);

    QStringList seasons;
    seasons << "spring" << "summer" << "autumn" << "winter";

    QAbstractSpinBox* spinBox = new QSpinBox(this);
    static_cast<QSpinBox*>(spinBox)->setRange(INT_MIN, INT_MAX);
    static_cast<QSpinBox*>(spinBox)->setValue(INT_MAX);
    static_cast<QSpinBox*>(spinBox)->setDisplayIntegerBase(16);
    insertWidget(spinBox);

    spinBox = new QtLongSpinBox(this);
    static_cast<QtLongSpinBox*>(spinBox)->setRange(LONG_LONG_MIN, LONG_LONG_MAX);
    static_cast<QtLongSpinBox*>(spinBox)->setValue(INT_MAX);
    static_cast<QtLongSpinBox*>(spinBox)->setDisplayIntegerBase(16);
    insertWidget(spinBox);

    spinBox = new QtTextSpinBox(seasons, this);
    insertWidget(spinBox);

    QtSpanSlider* vslider = new QtSpanSlider(Qt::Vertical, this);
    vslider->setTickInterval(10);
    vslider->setTickPosition(QSlider::TicksBothSides);
    insertWidget(vslider);

    QtSpanSlider* hslider = new QtSpanSlider(Qt::Horizontal, this);
    hslider->setHandleMovementMode(QtSpanSlider::NoCrossing);
    hslider->setTickInterval(4);
    hslider->setTickPosition(QSlider::TicksAbove);
    insertWidget(hslider);

    QtSlideButton* button = new QtSlideButton(this);
    insertWidget(button);
}

DemoWidget::~DemoWidget()
{

}

void DemoWidget::insertWidget(QWidget *w)
{
    //layout->addRow(Qt::className(w), w);
    QHBoxLayout* itemLayout = new QHBoxLayout;
    itemLayout->addWidget(new QLabel(Qt::className(w)));
    itemLayout->addWidget(w, 1);
    layout->addLayout(itemLayout);
}
