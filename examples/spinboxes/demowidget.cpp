#include "demowidget.h"
#include <QFormLayout>
#include <QBoxLayout>

#include <QLabel>
#include <QPushButton>

#include <QtGlobalExtra>

#include <QtLongSpinBox>
#include <QtTextSpinBox>
#include <QtSpinBoxEdit>
#include <QtSpanSlider>

#include <QtSlideButton>
#include <QtSlideCheckBox>

#include <QtCardWidget>

#include <QToolTip>

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

    spinBox = new QtSpinBoxEdit(this);
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

    QtSlideCheckBox* checkBoxHor = new QtSlideCheckBox(this);
    connect(checkBoxHor, SIGNAL(clicked(bool)), this, SLOT(clicked(bool)));
    //checkBoxHor->setMaximumSize(checkBoxHor->minimumSizeHint());
    insertWidget(checkBoxHor);

    QtSlideCheckBox* checkBoxVer = new QtSlideCheckBox(Qt::Vertical, this);
    connect(checkBoxVer, SIGNAL(clicked(bool)), this, SLOT(clicked(bool)));
    //checkBoxVer->setMaximumSize(checkBoxVer->minimumSizeHint());
    insertWidget(checkBoxVer);

    bageEffect = new QtGraphicsBageEffect(this);

    QFrame* frame = new QFrame(this);
    frame->setMinimumSize(64, 64);
    frame->setFrameStyle(QFrame::Plain | QFrame::Box);
    frame->setGraphicsEffect(bageEffect);
    bageEffect->setIcon(QIcon::fromTheme("user-available").pixmap(16, 16));
    insertWidget(frame);


    QSpinBox* bageSpin = new QSpinBox(this);
    bageSpin->setRange(-1, 1010);
    connect(bageSpin, SIGNAL(valueChanged(int)), bageEffect, SLOT(setCounter(int)));
    insertWidgets(new QLabel("Bage value"), bageSpin);
}

DemoWidget::~DemoWidget()
{

}

void DemoWidget::insertWidget(QWidget *w)
{
    QHBoxLayout* itemLayout = new QHBoxLayout;
    itemLayout->addWidget(new QLabel(QtExtra::className(w)));
    itemLayout->addWidget(w, 1);
    layout->addLayout(itemLayout);
}

void DemoWidget::insertWidgets(QWidget *label, QWidget* content)
{
    QHBoxLayout* itemLayout = new QHBoxLayout;
    itemLayout->addWidget(label);
    itemLayout->addWidget(content);
    layout->addLayout(itemLayout);
}

void DemoWidget::clicked(bool on)
{
    QToolTip::showText(this->pos(), tr(on ? "Enabled" : "Disabled"), 0, rect(), 2000);
}

void DemoWidget::setBageValue(int value)
{
    bageEffect->setCounter(value);
}


