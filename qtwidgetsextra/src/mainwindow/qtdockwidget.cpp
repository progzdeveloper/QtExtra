#include "qtdockwidget.h"
#include "qtdockwidgettitlebar.h"
#include <QEvent>

#include <QDebug>

class QtDockWidgetPrivate
{
public:
    QtDockWidget *q_ptr;
    QTimer *timer;
    bool autoHide;
    inline void init(QtDockWidget *w);
};

void QtDockWidgetPrivate::init( QtDockWidget *w )
{
    q_ptr = w;
    //q_ptr->setAttribute(Qt::WA_Hover);
    q_ptr->setMouseTracking(true);
    autoHide = false;
    w->setTitleBarWidget(new QtDockWidgetTitleBar(w));
}

QtDockWidget::QtDockWidget( QWidget *parent /*= 0*/, Qt::WindowFlags f /*= 0*/ ) 
    : QDockWidget(parent, f), d_ptr(new QtDockWidgetPrivate)
{
    d_ptr->init(this);
}

QtDockWidget::QtDockWidget( const QString& title, QWidget *parent /*= 0*/, Qt::WindowFlags f /*= 0*/ ) 
    : QDockWidget(title, parent, f), d_ptr(new QtDockWidgetPrivate)
{
    d_ptr->init(this);
}

QtDockWidget::~QtDockWidget()
{
    delete d_ptr;
}

void QtDockWidget::setAutoHide( bool on /*= true*/ )
{
    Q_D(QtDockWidget);
    d->autoHide = on;
    emit autoHideChanged(on);
}

bool QtDockWidget::isAutoHide() const
{
    Q_D(const QtDockWidget);
    return d->autoHide;
}

void QtDockWidget::leaveEvent( QEvent * e )
{
    Q_UNUSED(e)
    /*qDebug() << "Event type: " << e->type();
    if (isAutoHide() && e->type() == QEvent::HoverLeave) {

        hide();
    }*/
}

void QtDockWidget::focusOutEvent(QFocusEvent *e)
{
    Q_UNUSED(e)

    /*if (isAutoHide()) {
        hide();
    }*/
}
