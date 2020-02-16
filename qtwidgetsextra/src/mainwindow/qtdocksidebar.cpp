#include "qtdocksidebar.h"
#include "qtdocktabbar.h"
#include "qtdockwidget.h"

#include <QTabBar>
#include <QDockWidget>
#include <QMouseEvent>
#include <QIcon>
#include <QList>


inline QString dockSideBarTitle(Qt::DockWidgetArea area)
{
    switch(area) {
    case Qt::LeftDockWidgetArea:
        return "_qt_LeftSideBar";
    case Qt::RightDockWidgetArea:
        return "_qt_RightSideBar";
    case Qt::TopDockWidgetArea:
        return "_qt_TopSideBar";
    case Qt::BottomDockWidgetArea:
        return "_qt_BottomSideBar";
    default:
        break;
    }
    return "_qt_SideBar";
}


class QtDockSideBarPrivate
{
public:
    QtDockSideBar *q_ptr;
    QList<QtDockWidget*> widgets;
    QtDockTabBar *tabBar;
    QtDockSideBarPrivate(QtDockSideBar* q);
    void initUi(Qt::DockWidgetArea area);
};


QtDockSideBarPrivate::QtDockSideBarPrivate(QtDockSideBar *q) :
    q_ptr(q)
{
}

void QtDockSideBarPrivate::initUi(Qt::DockWidgetArea area)
{
    tabBar = new QtDockTabBar(area, q_ptr);

    q_ptr->insertWidget(0, tabBar);
    q_ptr->setMovable(false);
    q_ptr->setFloatable(false);

    QObject::connect(tabBar, SIGNAL(tabClicked(int)), q_ptr, SLOT(show(int)));
    QObject::connect(tabBar, SIGNAL(tabHover(int)), q_ptr, SLOT(show(int)));
    QObject::connect(tabBar, SIGNAL(tabMoved(int, int)), q_ptr, SLOT(moveTab(int, int)));
}


QtDockSideBar::QtDockSideBar(Qt::DockWidgetArea area, QWidget *parent) :
    QToolBar(dockSideBarTitle(area), parent),
    d_ptr(new QtDockSideBarPrivate(this))
{
    d_ptr->initUi(area);
    setMovable(false);
    setFloatable(false);
    hide();
    setObjectName("_qt_sideBar");
}

QtDockSideBar::~QtDockSideBar()
{
    delete d_ptr;
}


void QtDockSideBar::insertDockWidget(QtDockWidget *dw)
{
    Q_D(QtDockSideBar);
    if (contains(dw))
        return;

    if (dw->isAutoHide()) {
        d->tabBar->addTab(dw->windowIcon(), dw->windowTitle());
        QToolBar::show();
    }
    d->widgets << dw;
}

void QtDockSideBar::removeDockWidget(QtDockWidget *dw)
{
    Q_D(QtDockSideBar);
    int i = d->widgets.indexOf(dw);

    if (i != -1) {
        d->widgets.removeAt(i);
        d->tabBar->removeTab(i);
    }

    if (d->tabBar->count() == 0)
        hide();
}

bool QtDockSideBar::contains(QtDockWidget *w) const
{
    Q_D(const QtDockSideBar);
    return (d->widgets.indexOf(w) != -1);
}

void QtDockSideBar::moveTab(int from, int to)
{
    Q_D(QtDockSideBar);
    d->widgets.move(from, to);
}

void QtDockSideBar::show(int key)
{
    Q_D(QtDockSideBar);
    QList<QtDockWidget*>::iterator it = d->widgets.begin();
    for (; it != d->widgets.end(); ++it) {
        if ((*it)->isAutoHide())
            (*it)->hide();
    }
    if (key >= 0 && key < d->widgets.size()) {
        d->widgets[key]->show();
        d->widgets[key]->setFocus();
        QWidget *content = d->widgets[key]->widget();
        if (content)
            content->setFocus();
    }
}

