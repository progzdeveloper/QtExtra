#include "qtdocktabbar.h"
#include <QMouseEvent>


QtDockTabBar::QtDockTabBar( Qt::DockWidgetArea area, QWidget *parent /*= 0*/ ) : QTabBar(parent)
{
    switch(area)
    {
    case Qt::TopDockWidgetArea:
        setShape(QTabBar::RoundedSouth);
        break;
    case Qt::BottomDockWidgetArea:
        setShape(QTabBar::RoundedNorth);
        break;
    case Qt::LeftDockWidgetArea:
        setShape(QTabBar::RoundedEast);
        break;
    case Qt::RightDockWidgetArea:
        setShape(QTabBar::RoundedWest);
        break;
    case Qt::NoDockWidgetArea:
    case Qt::AllDockWidgetAreas:
        break;
    }
    setMouseTracking(true);
    setExpanding(false);
    setDrawBase(false);
    setDocumentMode(false);
    setMovable(true);
}

QtDockTabBar::~QtDockTabBar()
{
}

void QtDockTabBar::mousePressEvent( QMouseEvent * e )
{
    int index = tabAt(e->pos());
    if (index != -1)
        emit tabClicked(index);
    QTabBar::mousePressEvent(e);
}

void QtDockTabBar::mouseMoveEvent( QMouseEvent * e )
{
    int index = tabAt(e->pos());
    if (index != currentIndex()) {
        setCurrentIndex(index);
        emit tabHover(index);
    }
    QTabBar::mouseMoveEvent(e);
}

void QtDockTabBar::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Space)
        emit tabClicked(currentIndex());
    QTabBar::keyPressEvent(e);
}

