#include <QPointer>
#include <QList>

#include <QPainter>
#include <QFont>

#include <QtWidgets>

#include "qtwindowmanager.h"

class QWidgetListItem : 
        public QListWidgetItem
{
public:
    QWidgetListItem(QWidget* widget, QListWidget* view, bool specific = false) :
        QListWidgetItem(view), w(widget), isSpecific(specific)
    {
    }

    QVariant data(int role) const
    {
        if(!w) {
            return QListWidgetItem::data(role);
        }

        QFont font = qApp->font();
        font.setBold(isSelected());
        font.setItalic(isSpecific);
        switch(role)
        {
        case Qt::DisplayRole:
            return (w->windowTitle().isEmpty() ? w->objectName() : w->windowTitle());
        case Qt::ToolTipRole:
            return w->toolTip();
        case Qt::StatusTipRole:
            return w->statusTip();
        case Qt::DecorationRole:
            return w->windowIcon();
        case Qt::FontRole:
            return font;
        }
        return QListWidgetItem::data(role);
    }

    QWidget* widget() const { return w; }

private:
    QPointer<QWidget> w;
    bool isSpecific;
};



class QtWindowManagerPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtWindowManagerPrivate)
public:
    QtWindowManager* q_ptr;
    QPointer<QMainWindow> window;
    QPointer<QWidget> centralWidget;

    QListWidget* dwList;
    QListWidget* mdiList;
    QListWidget* activeList;
    QLabel *dwLabel;
    QLabel *mdiLabel;

    bool highlight;

    void initUi();
    void updateContents();
    void prevItem();
    void nextItem();
    void switchWidgetList();
    void activateWindow(QWidget* w);

    static void moveToCenter(QWidget* w, QWidget* parent = 0);
private:
    template<class Widget>
    static inline void collectWidgets(QListWidget* view, const QList<Widget>& widgets, bool flagVisible);

    static inline QListWidgetItem* createItem(QListWidget* view, QWidget* w, bool isCentral = false);
    static inline QListWidget* createList(QWidget* parent);
};

void QtWindowManagerPrivate::initUi()
{
    dwLabel = new QLabel(tr("<b>Dock Widgets</b>"), q_ptr);
    dwLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    mdiLabel = new QLabel(tr("<b>Active Windows</b>"), q_ptr);
    mdiLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    dwList = createList(q_ptr);
    mdiList = createList(q_ptr);

    QVBoxLayout* dwLayout = new QVBoxLayout;
    dwLayout->addWidget(dwLabel);
    dwLayout->addWidget(dwList);

    QVBoxLayout* mdiLayout = new QVBoxLayout;
    mdiLayout->addWidget(mdiLabel);
    mdiLayout->addWidget(mdiList);

    QHBoxLayout *layout = new QHBoxLayout(q_ptr);
    layout->addLayout(dwLayout);
    layout->addLayout(mdiLayout);

    QObject::connect(mdiList, SIGNAL(itemClicked(QListWidgetItem*)),
                     q_ptr, SLOT(itemClicked(QListWidgetItem*)));
    QObject::connect(dwList, SIGNAL(itemClicked(QListWidgetItem*)),
                     q_ptr, SLOT(itemClicked(QListWidgetItem*)));
    activeList = mdiList;
}

void QtWindowManagerPrivate::updateContents()
{
    dwList->clear();
    mdiList->clear();
    if (!window)
        return;

    QMdiArea* area = qobject_cast<QMdiArea*>(window->centralWidget());
    if (!area) {
        dwLabel->setText(tr("<b>Windows</b>"));
        createItem(dwList, window->centralWidget(), true);
    } else {
        collectWidgets(mdiList, area->subWindowList(), false);
        QWidget* actveWidget = area->activeSubWindow();
        for (int i = 0; i < mdiList->count(); i++) {
            if (actveWidget == static_cast<QWidgetListItem*>(mdiList->item(i))->widget()) {
                mdiList->setCurrentRow(i);
            }
        }
    }

    collectWidgets(dwList, window->findChildren<QDockWidget*>(), true);
    if (dwList->count() != 0) {
        dwList->setCurrentRow(0);
    }
}


template<class Widget>
inline void QtWindowManagerPrivate::collectWidgets(QListWidget* view, const QList<Widget>& widgets, bool flagVisible)
{
    typedef QList<Widget> WidgetList;
    typename WidgetList::const_iterator it = widgets.begin();
    for (; it != widgets.end(); ++it)
    {
        if (flagVisible) {
            if ((*it)->isVisible())
                createItem(view, *it);
        } else {
            createItem(view, *it);
        }
    }
}


inline QListWidgetItem* QtWindowManagerPrivate::createItem(QListWidget* view, QWidget* w, bool specific)
{
    return new QWidgetListItem(w, view, specific);
}

inline QListWidget* QtWindowManagerPrivate::createList(QWidget* parent)
{
    QPalette palette = qApp->palette();
    palette.setBrush(QPalette::Base, palette.window());

    QListWidget* view = new QListWidget(parent);
    view->setCursor(Qt::PointingHandCursor);
    view->setFrameShape(QFrame::NoFrame);
    view->setResizeMode(QListView::Adjust);
    view->setMouseTracking(true);
    view->setPalette(palette);

    return view;
}


void QtWindowManagerPrivate::prevItem()
{
    int row = activeList->currentRow();
    if (row == 0)
        row = activeList->count()-1;
    else
        --row;
    activeList->setCurrentRow(row);
}

void QtWindowManagerPrivate::nextItem()
{
    int row = activeList->currentRow();
    if (row == activeList->count()-1)
        row = 0;
    else
        ++row;
    activeList->setCurrentRow(row);
}

void QtWindowManagerPrivate::switchWidgetList()
{
    activeList = (activeList == mdiList ? dwList : mdiList);
    activeList->setFocus();
    q_ptr->repaint();
}

void QtWindowManagerPrivate::activateWindow(QWidget* widget)
{
    QMdiSubWindow *subWindow = qobject_cast<QMdiSubWindow*>(widget);
    if (subWindow) {
        QMdiArea* area = qobject_cast<QMdiArea*>(centralWidget);
        if (area)
            area->setActiveSubWindow(subWindow);
        return;
    }

    QDockWidget* dockWidget = qobject_cast<QDockWidget*>(widget);
    if (dockWidget && dockWidget->widget()) {
        dockWidget->raise();
        dockWidget->activateWindow();
        dockWidget->widget()->setFocus();
        return;
    }

    widget->raise();
    widget->setFocus();
}

void QtWindowManagerPrivate::moveToCenter(QWidget* w, QWidget* parent)
{
    parent = (parent == 0 ? qApp->desktop() : parent);
    if (parent) {
        int x = parent->x() + (parent->width()/2 - w->width()/2);
        int y = parent->y() + (parent->height()/2 - w->height()/2);
        w->move(x, y);
    }
}





QtWindowManager::QtWindowManager(QWidget* parent /*= 0*/, Qt::WindowFlags flags /*= 0*/) :
    QFrame(parent, flags),
    d_ptr(new QtWindowManagerPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->window = 0;
    d_ptr->centralWidget = 0;
    d_ptr->highlight = false;
    d_ptr->initUi();
    setFrameStyle(QFrame::Box|QFrame::Plain);
}

QtWindowManager::~QtWindowManager()
{
    delete d_ptr;
}

void QtWindowManager::setMainWindow(QMainWindow* w)
{
    Q_D(QtWindowManager);
    if (!w)
        return;

    d->window = w;
    d->centralWidget = d->window->centralWidget();
    QMdiArea* area = qobject_cast<QMdiArea*>(d->centralWidget);
    if (area) {
        qApp->removeEventFilter(area);
    }
    qApp->installEventFilter(this);
}

QMainWindow* QtWindowManager::mainWindow() const
{
    Q_D(const QtWindowManager);
    return d->window;
}

void QtWindowManager::setHighlightEnabled(bool on)
{
    Q_D(QtWindowManager);
    d->highlight = on;
}

bool QtWindowManager::isHighlightEnaled() const
{
    Q_D(const QtWindowManager);
    return d->highlight;
}

void QtWindowManager::paintEvent(QPaintEvent* e)
{
    Q_D(QtWindowManager);
    QFrame::paintEvent(e);
    if (d->highlight) {
        QPainter painter(this);
        painter.setPen(Qt::gray);
        painter.drawRect(d->activeList->geometry().adjusted(-1, -1, 0, 0));
    }
}

void QtWindowManager::showEvent(QShowEvent* e)
{
    Q_D(QtWindowManager);

    activateWindow();
    d->updateContents();

    d->dwList->setEnabled(d->dwList->count() != 0);
    d->mdiList->setEnabled(d->mdiList->count() != 0);

    d->dwList->adjustSize();
    d->mdiList->adjustSize();

    QMdiArea* area = qobject_cast<QMdiArea*>(d->centralWidget);
    d->mdiList->setVisible(area != 0);
    d->mdiLabel->setVisible(area != 0);
    d->activeList = (d->mdiList->isEnabled() ? d->mdiList : d->dwList);

    d->activeList->setFocus();

    d->moveToCenter(this, parentWidget());

    adjustSize();
    QWidget::showEvent(e);
}

void QtWindowManager::hideEvent(QHideEvent* e)
{
    Q_D(QtWindowManager);
    QWidgetListItem* item = static_cast<QWidgetListItem*>(d->activeList->currentItem());
    if (!item)
        return;

    d->activateWindow(item->widget());
    QWidget::hideEvent(e);
}

bool QtWindowManager::eventFilter(QObject* object, QEvent* e)
{
    Q_D(QtWindowManager);
    if (e->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
        if ( !geometry().contains( mouseEvent->globalPos() ) ) {
            hide();
        }
    }

    if (e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
        if (object == d->window || object == this)
            return filterKeyEvent(object, keyEvent);

        QMdiArea* area = qobject_cast<QMdiArea*>(d->centralWidget);
        if (!area) {
            return filterKeyEvent(object, keyEvent);
        }
        QList<QMdiSubWindow*> subwindows = area->subWindowList();
        QList<QMdiSubWindow*>::const_iterator swIt = subwindows.begin();
        for (; swIt != subwindows.end(); ++swIt) {
            if ((*swIt)->widget() == object)
                return filterKeyEvent(object, keyEvent);
        }
        QList<QDockWidget*> dockWidgets = d->window->findChildren<QDockWidget*>();
        QList<QDockWidget*>::const_iterator dwIt = dockWidgets.begin();
        for (; dwIt != dockWidgets.end(); ++dwIt) {
            if ((*dwIt)->widget() == object)
                return filterKeyEvent(object, keyEvent);
        }
    }

    return false;
}


bool QtWindowManager::filterKeyEvent(QObject* object, QKeyEvent* e)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
    if (e->type() == QEvent::KeyPress && keyEvent->modifiers() == Qt::ControlModifier) {
        return (execAction(keyEvent->key()));
    }
    if (e->type() == QEvent::KeyRelease && keyEvent->key() == Qt::Key_Control && isVisible()) {
        hide();
        return true;
    }
    return QWidget::eventFilter(object, e);
}


bool QtWindowManager::execAction(int key)
{
    Q_D(QtWindowManager);
    switch(key)
    {
    case Qt::Key_Tab:
        if (!isVisible())
            show();
        else
            d->nextItem();
        return true;
    case Qt::Key_Left:
    case Qt::Key_Right:
        d->switchWidgetList();
        return true;
    case Qt::Key_Up:
        d->prevItem();
        return true;
    case Qt::Key_Down:
        d->nextItem();
        return true;
    }
    return false;
}


void QtWindowManager::itemClicked(QListWidgetItem*)
{
    Q_D(QtWindowManager);
    d->activeList = qobject_cast<QListWidget*>(sender());
    hide();
}

