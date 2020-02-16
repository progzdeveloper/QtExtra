#include "qtoptionswidget.h"
#include <QDialogButtonBox>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>

class QtOptionsWidgetPrivate
{
public:
    QListWidget *pageList;
    QStackedWidget *content;
};




QtOptionsWidget::QtOptionsWidget( QWidget *parent /*= 0*/ )
    : QWidget(parent), d_ptr(new QtOptionsWidgetPrivate)
{
    d_ptr->content = new QStackedWidget(this);
    d_ptr->pageList = new QListWidget(this);
    d_ptr->pageList->setMaximumWidth(150);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d_ptr->pageList);
    layout->addWidget(d_ptr->content);
    layout->setMargin(0);

    connect(d_ptr->pageList,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

QtOptionsWidget::~QtOptionsWidget()
{
}

int QtOptionsWidget::addWidget( QWidget * widget )
{
    Q_D(QtOptionsWidget);
    new QListWidgetItem(widget->windowTitle(), d->pageList);
    return d->content->addWidget(widget);
}

int QtOptionsWidget::count() const
{
    Q_D(const QtOptionsWidget);
    return d->content->count();
}

int QtOptionsWidget::currentIndex() const
{
    Q_D(const QtOptionsWidget);
    return d->content->currentIndex();
}

QWidget * QtOptionsWidget::currentWidget() const
{
    Q_D(const QtOptionsWidget);
    return d->content->currentWidget();
}

int QtOptionsWidget::indexOf( QWidget * widget ) const
{
    Q_D(const QtOptionsWidget);
    return d->content->indexOf(widget);
}

QWidget * QtOptionsWidget::widget( int index ) const
{
    Q_D(const QtOptionsWidget);
    return d->content->widget(index);
}

void QtOptionsWidget::setCurrentIndex( int index )
{
    Q_D(QtOptionsWidget);
    d->content->setCurrentIndex(index);
    d->pageList->setCurrentRow(index);
}

void QtOptionsWidget::setCurrentWidget( QWidget * widget )
{
    Q_D(QtOptionsWidget);
    d->content->setCurrentWidget(widget);
    d->pageList->setCurrentRow(d->content->indexOf(widget));
}

void QtOptionsWidget::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_D(QtOptionsWidget);
    if (!current)
        current = previous;

    d->content->setCurrentIndex(d->pageList->row(current));
}
