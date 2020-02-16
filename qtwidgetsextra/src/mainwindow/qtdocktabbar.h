#pragma once
#include <QTabBar>

class QKeyEvent;
class QMouseEvent;

class  QtDockTabBar : 
        public QTabBar
{
    Q_OBJECT
public:
    explicit QtDockTabBar(Qt::DockWidgetArea area, QWidget *parent = 0);
    ~QtDockTabBar();

signals:
    void tabClicked(int i);
    void tabHover(int i);

protected:
    void mouseMoveEvent ( QMouseEvent * e );
    void mousePressEvent ( QMouseEvent * e );
    void keyPressEvent( QKeyEvent *e );
private:
    friend class QtSideBar;
    Q_DISABLE_COPY(QtDockTabBar)
};
