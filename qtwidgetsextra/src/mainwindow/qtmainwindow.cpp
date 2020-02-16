#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QScopedPointer>
#include <QMap>
#include <QByteArray>
#include <QDataStream>
#include <QApplication>

#include "qtmainwindow.h"
#include "qtdocksidebar.h"
#include "qtdockwidget.h"
#include "qtdockwidgettitlebar.h"

typedef QMap<int, QtDockSideBar*> QtSideBars;

class QtMainWindowPrivate
{
public:
    QtMainWindow *q_ptr;
    QtSideBars dockBars;
    QMap<Qt::DockWidgetArea, Qt::ToolBarArea> dockSideMap;
    QMap<Qt::ToolBarArea, Qt::DockWidgetArea> sideDockMap;

    QtMainWindowPrivate(QtMainWindow* q);

    void createDockBars();
    QtDockSideBar* createSideBar(Qt::DockWidgetArea side);

    void createToolBarMenu(QMenu *menu);
    void createDockWidgetMenu(QMenu* menu);
};


QtMainWindowPrivate::QtMainWindowPrivate(QtMainWindow *q) : q_ptr(q) {
}

void QtMainWindowPrivate::createDockBars()
{
    dockSideMap[Qt::LeftDockWidgetArea] = Qt::LeftToolBarArea;
    dockSideMap[Qt::RightDockWidgetArea] = Qt::RightToolBarArea;
    dockSideMap[Qt::TopDockWidgetArea] = Qt::TopToolBarArea;
    dockSideMap[Qt::BottomDockWidgetArea] = Qt::BottomToolBarArea;

    sideDockMap[Qt::LeftToolBarArea] = Qt::LeftDockWidgetArea;
    sideDockMap[Qt::RightToolBarArea] = Qt::RightDockWidgetArea;
    sideDockMap[Qt::TopToolBarArea] = Qt::TopDockWidgetArea;
    sideDockMap[Qt::BottomToolBarArea] = Qt::BottomDockWidgetArea;

    dockBars[Qt::LeftDockWidgetArea] = createSideBar(Qt::LeftDockWidgetArea);
    dockBars[Qt::RightDockWidgetArea] = createSideBar(Qt::RightDockWidgetArea);
    dockBars[Qt::TopDockWidgetArea] = createSideBar(Qt::TopDockWidgetArea);
    dockBars[Qt::BottomDockWidgetArea] = createSideBar(Qt::BottomDockWidgetArea);
}

QtDockSideBar* QtMainWindowPrivate::createSideBar(Qt::DockWidgetArea area)
{
    QtDockSideBar *sideBar = new QtDockSideBar(area, q_ptr);
    q_ptr->addToolBar(dockSideMap[area], sideBar);
    return sideBar;
}

void QtMainWindowPrivate::createToolBarMenu(QMenu *menu)
{
    typedef QList<QToolBar*> QToolBarList;
    QString title;
    QAction * action = 0;
    QToolBarList toolbars = q_ptr->findChildren<QToolBar*>();
    QToolBarList::const_iterator it = toolbars.begin();
    for (; it != toolbars.end(); ++it) {
        title = (*it)->windowTitle();
        if (title.startsWith("_qt_"))
            continue;

        action = menu->addAction(title, *it, SLOT(setVisible(bool)));
        action->setCheckable(true);
        action->setChecked((*it)->isVisible());
    }
}

void QtMainWindowPrivate::createDockWidgetMenu(QMenu* menu)
{
    typedef QList<QDockWidget*> QDockWidgetList;
    QAction * action = 0;
    QDockWidgetList widgets = q_ptr->findChildren<QDockWidget*>();
    QDockWidgetList::const_iterator it = widgets.begin();
    for (; it != widgets.end(); ++it) {
        action = menu->addAction((*it)->windowIcon(), (*it)->windowTitle(), *it, SLOT(setVisible(bool)));
        action->setCheckable(true);
        action->setChecked((*it)->isVisible());
    }
}


static inline bool isAncestor(QWidget *w, QWidget *child, bool checkType = false)
{
    while (child) {
        if (child == w)
            return true;
        if (checkType) {
            if (child->isWindow())
                return false;
        }
        child = child->parentWidget();
    }
    return false;
}


QtMainWindow::QtMainWindow(QWidget *parent, Qt::WindowFlags f) :
    QMainWindow(parent, f), d_ptr(new QtMainWindowPrivate(this))
{
    Q_D(QtMainWindow);
    d->createDockBars();
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
            this, SLOT(focusChanged(QWidget*, QWidget*)));
}

QtMainWindow::~QtMainWindow()
{
}

void QtMainWindow::addDockWidget(Qt::DockWidgetArea area, QtDockWidget * dockwidget)
{
    addDockWidget(area, dockwidget, Qt::Vertical);
}

void QtMainWindow::addDockWidget(Qt::DockWidgetArea area, QtDockWidget * dockwidget, Qt::Orientation orientation)
{
    connect(dockwidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(changeDockLocation(Qt::DockWidgetArea)));
    connect(dockwidget, SIGNAL(autoHideChanged(bool)),
            this, SLOT(changeDockState(bool)));

    QMainWindow::addDockWidget(area, dockwidget, orientation);
}

void QtMainWindow::changeDockLocation(Qt::DockWidgetArea area)
{
    Q_D(QtMainWindow);
    QtDockWidget * w = qobject_cast<QtDockWidget*>(sender());
    if (w) {
        QtSideBars::const_iterator it = d->dockBars.begin();
        for (; it != d->dockBars.end(); ++it) {
            QtDockSideBar *sideBar = (*it);
            if (sideBar->contains(w)) {
                sideBar->removeDockWidget(w);
                d->dockBars[area]->insertDockWidget(w);
                return;
            }
        }
    }
}

void QtMainWindow::changeDockState(bool on)
{
    Q_D(QtMainWindow);
    QtDockWidget * w = qobject_cast<QtDockWidget*>(sender());
    if (!w)
        return;
    Qt::DockWidgetArea area = dockWidgetArea(w);
    if (on) {
        d->dockBars[area]->insertDockWidget(w);
    } else {
        d->dockBars[area]->removeDockWidget(w);
        w->show();
    }
}

QMenu *QtMainWindow::createContextMenu()
{
    Q_D(QtMainWindow);
    QMenu *menu = new QMenu(this);
    d->createToolBarMenu(menu);
    menu->addSeparator();
    d->createDockWidgetMenu(menu);
    return menu;
}


void QtMainWindow::contextMenuEvent(QContextMenuEvent *e)
{
    QWidget * w = menuWidget();
    if ( w && w->rect().contains(e->pos()) ) {
        QScopedPointer<QMenu> menu(createContextMenu());
        menu->exec(e->globalPos());
    }
}

void QtMainWindow::focusChanged(QWidget * old, QWidget * now )
{
    if (!now || !old)
        return;

    typedef QList<QtDockWidget*> QtDockWidgetList;
    QtDockWidget *dw = 0;
    QtDockWidgetList widgets = findChildren<QtDockWidget*>();
    QtDockWidgetList::const_iterator it = widgets.begin();
    for (; it != widgets.end(); ++it)
    {
        dw = *it;
        if (dw && dw->isAutoHide())
        {
            int wtype = now->windowType();
            if (wtype == Qt::ToolTip || // current focus on tooltip
                wtype == Qt::Popup ||  // current focus on popup menu
                wtype == Qt::Window) { // current focus on some window
                continue;
            }

            // TODO: implement using QWidget::isAncestorOf(...)
            if ((!isAncestor(dw, now) && now != dw) ||
                    now == centralWidget()) {
                dw->hide();
            }
        }
    }
}

QByteArray QtMainWindow::saveDockState() const
{
    typedef QList<QtDockWidget*> QtDockWidgetList;

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    QDataStream ds(&buffer);

    QtDockWidget *dw = 0;
    QtDockWidgetList widgets = findChildren<QtDockWidget*>();
    ds << widgets.count();
    QtDockWidgetList::const_iterator it = widgets.begin();
    for (; it != widgets.end(); ++it) {
        dw = *it;
        ds << dw->objectName();
        ds << dw->isAutoHide();
    }
    return bytes;
}

void QtMainWindow::restoreDockState(const QByteArray& bytes)
{
    QByteArray ba = bytes;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::ReadOnly);
    QDataStream ds(&buffer);
    int count = 0;
    bool autoHide;
    QString objectName;
    QtDockWidget *dw = 0;
    ds >> count;
    while(count--) {
        ds >> objectName;
        ds >> autoHide;
        dw = findChild<QtDockWidget*>(objectName);
        if (dw) {
            dw->setAutoHide(autoHide);
        }
    }
}


