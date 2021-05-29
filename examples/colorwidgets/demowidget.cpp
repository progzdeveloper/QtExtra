#include "demowidget.h"
#include <QFormLayout>
#include <QBoxLayout>

#include <QLabel>

#include <QtGlobalExtra>

#include <QtColorSelector>
#include <QtColorButton>
#include <QtColorComboBox>
#include <QtColorTriangle>
#include <QtHsvRectPicker>
#include <QtHsvRadialPicker>



DemoWidget::DemoWidget(QWidget *parent)
    : QWidget(parent)
{
    //layout = new QFormLayout(this);
    layout = new QVBoxLayout(this);


    insertWidget(new QtColorSelector(this));


    QtColorButton* colorButton = new QtColorButton(this);
    colorButton->setIconSize({48, 48});
    colorButton->setGridWidth(colorButton->iconSize().width() * 5 + 3);
    colorButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    colorButton->setColor(Qt::white);
    colorButton->updateMenu();
    insertWidget(colorButton);

    QtColorComboBox* colorCombo = new QtColorComboBox(this);
    colorCombo->setStandardColors();
    colorCombo->setColorDialogEnabled(true);
    insertWidget(colorCombo);


    insertWidget(new QtHsvRectPicker(this));
    insertWidget(new QtHsvRadialPicker(this));

    insertWidget(new QtColorTriangle(this));

}

DemoWidget::~DemoWidget()
{

}

void DemoWidget::insertWidget(QWidget *w)
{
    //layout->addRow(Qt::className(w), w);
    QHBoxLayout* itemLayout = new QHBoxLayout;
    itemLayout->addWidget(new QLabel(QtExtra::className(w)));
    itemLayout->addWidget(w, 1);
    layout->addLayout(itemLayout);
}
