#include "widget.h"
#include <QDesktopServices>
#include <QUrl>
#include <QBoxLayout>
#include <QHeaderView>
#include <QAbstractItemModel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QDebug>

#include <QtPlaceholderEffect>
#include <QtProgressEffect>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(10000);
    connect(timer, SIGNAL(timeout()), SLOT(cancel()));
    animation = new QPropertyAnimation(this);

    view = new QTableWidget(0, 2, this);
    view->setHorizontalHeaderLabels(QStringList() << tr("Key") << tr("Value"));
    view->setSelectionBehavior(QTableWidget::SelectRows);

    QtPlaceholderEffect* effect = new QtPlaceholderEffect(view);
    effect->setPlaceholderText(tr("<p><i>Nothing to display</i></p><p>Click to this <a href=\"http://www.mail.ru\">link</a>.</p>"));
    connect(effect, SIGNAL(linkActivated(QString)), SLOT(openLink(QString)));

    progressEffect = new QtProgressEffect(view, view);
    progressEffect->setEnabled(false);
    progressEffect->setLabelText("Connecting with database...");

    animation->setTargetObject(progressEffect);
    animation->setDirection(QAbstractAnimation::Forward);
    animation->setPropertyName("opacity");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setDuration(300);
    connect(progressEffect, SIGNAL(enabledChanged(bool)), this, SLOT(toggleAnimation(bool)));

    //new QtPlaceholderEffect(view, tr("Nothing to display"));
    addRowButton = new QPushButton(tr("New Row..."));
    connect(addRowButton, SIGNAL(clicked()), this, SLOT(insertRow()));

    delRowButton = new QPushButton(tr("Remove Row..."));
    connect(delRowButton, SIGNAL(clicked()), this, SLOT(removeRow()));

    processButton = new QPushButton(tr("Process..."));
    connect(processButton, SIGNAL(clicked()), this, SLOT(process()));

    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setEnabled(false);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    QVBoxLayout* buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(addRowButton);
    buttonLayout->addWidget(delRowButton);
    buttonLayout->addWidget(processButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(view, 1);
    layout->addLayout(buttonLayout);
}

Widget::~Widget()
{
}

void Widget::insertRow()
{
    int row = view->currentRow();
    view->model()->insertRow(row == -1 ? 0 : row);
}

void Widget::removeRow()
{
    int row = view->currentRow();
    if (row != -1)
        view->model()->removeRow(row);
}

void Widget::process()
{
    cancelButton->setEnabled(true);
    addRowButton->setEnabled(false);
    delRowButton->setEnabled(false);
    view->setEnabled(false);
    view->horizontalHeader()->setVisible(false);
    view->verticalHeader()->setVisible(false);

    progressEffect->setEnabled(true);
    view->repaint();
    timer->start();
}

void Widget::cancel()
{
    cancelButton->setEnabled(false);
    addRowButton->setEnabled(true);
    delRowButton->setEnabled(true);
    view->setEnabled(true);
    view->horizontalHeader()->setVisible(true);
    view->verticalHeader()->setVisible(true);

    progressEffect->setEnabled(false);
    view->repaint();
}

void Widget::toggleAnimation(bool on)
{
    animation->setDirection(on ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    animation->start();
}

void Widget::openLink(const QString &url)
{
    process();
    if (!QDesktopServices::openUrl(url)) {
        qWarning() << "failed to open URL: " << url;
    }

}






