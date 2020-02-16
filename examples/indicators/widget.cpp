#include "widget.h"
#include <QtStateIndicator>
#include <QtSwitchButton>

#include <QSplitter>
#include <QVBoxLayout>

#include <QtPropertyWidget>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    indicator = new QtSwitchButton(this);
    //indicator = new QtStateIndicator(this);
    //qobject_cast<QtStateIndicator*>(indicator)->setClickable(false);
    //qobject_cast<QtStateIndicator*>(indicator)->setAnimated(true);
    //qobject_cast<QtStateIndicator*>(indicator)->setDuration(300);

    connect(indicator, SIGNAL(clicked()), SLOT(switchLight()));
    //indicator = new QtSwitchButton(this);

    browser = new QtPropertyWidget(this);
    browser->setObject(indicator);

    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(indicator);
    splitter->addWidget(browser);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(splitter);
}

Widget::~Widget()
{

}

void Widget::switchLight()
{
    QtStateIndicator* stateIndicator = qobject_cast<QtStateIndicator*>(indicator);
    if (!stateIndicator) {
        return;
    }
    QColor color = stateIndicator->color();
    QRgb rgb = color.rgb();
    rgb ^= qRgb(255, 255, 0);
    color.setRgb(rgb);
    stateIndicator->setColor(color);
}
