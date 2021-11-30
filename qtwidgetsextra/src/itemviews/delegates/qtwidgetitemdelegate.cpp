#include <memory>

#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QPixmapCache>
#include <QApplication>
#include <QAbstractItemView>
#include <QAbstractButton>
#include <QDateTime>
#include <QDebug>

#include "qtwidgetitemdelegate.h"

namespace
{
Q_CONSTEXPR const char* isButtonDownProperty = "down";

// Compute hash of model index and size of it cell representation.
// Hashing is doing by first packing row/column of index
// and width/height of size into 64-bit unsigned integers
// and reinterpreting them as a QLatin-1 string: this is
// one of the fastest methods
QString hashIndex(const QModelIndex& index, const QSize& size)
{
    char buf[2 * sizeof(uint64_t)];

    uint64_t h1 = uint64_t(index.row()) | uint64_t(index.column()) << 32; // pack index
    memcpy(buf + sizeof(uint64_t) * 0, &h1, sizeof(uint64_t)); // a la std::bit_cast

    uint64_t h2 = uint64_t(size.width()) | uint64_t(size.height()) << 32; // pack size
    memcpy(buf + sizeof(uint64_t) * 1, &h2, sizeof(uint64_t)); // a la std::bit_cast

   return QLatin1String(buf, sizeof(buf)); // reinterpret as a string
}

}

//
// struct WidgetState is a little helper
// that holds sub-widget and it's state that
// currently under mouse. The main aim of
// that struct is to persist sub-widget state
// between QtWidgetItemDelegate::paint() calls.
// Currently only QAbstractButton derived
// classes are supported.
//
struct WidgetState
{
    QWidget* control; // current sub-widget that under mouse cursor
    QStyle::State state; // current sub-widget state

    WidgetState() : control(nullptr), state(QStyle::State_None) {}
};

class QtWidgetItemDelegatePrivate
{
public:
    mutable WidgetState currentState; // current sub-widget state (sub-widget that under mouse cursor)
    mutable QModelIndex currentIndex; // current model index (index that under mouse cursor)
    mutable std::unique_ptr<QtItemWidget> widget; // widget to embed
    QPixmapCache cache;
    QWidget::RenderFlags flags; // widget rendering flags
    bool cacheEnabled : 1; // does cachild enabled
    bool staticContents : 1; // hint for static contents (in that case we don't need to handle sub-widget state)
    bool eventFilterEnabled : 1; // use QObject::eventFilter() or QStyleItemDelegate::editorEvent() for external event handling

    QtWidgetItemDelegatePrivate()
        : flags(QWidget::DrawChildren | QWidget::DrawWindowBackground)
        , cacheEnabled(true)
        , staticContents(true)
        , eventFilterEnabled(false)
    {}

    bool findPixmap(const QString& key, QPixmap& p) const
    {
        // here we can use more elaborated cache,
        // but for simplicity we currently use QPixmapCache
        return cache.find(key, &p);
    }

    void cachePixmap(const QString& key, const QPixmap& p) const
    {
        // here we can use more elaborated cache,
        // but for simplicity we currently use QPixmapCache
        cache.insert(key, p);
    }

    void renderDirect(QPainter* painter, const QRect& rect) const;
    void renderCached(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QtWidgetItemDelegate *delegate) const;

    void clearState() const;
    void resetState() const;
    void applyState() const;
    void resetWidget() const;
};


void QtWidgetItemDelegatePrivate::renderDirect(QPainter *painter, const QRect &rect) const
{
    painter->save();
    painter->translate(rect.topLeft()); // translate painter
    widget->render(painter, QPoint(), QRegion(), flags); // render our embeded widget
    painter->restore();
}

void QtWidgetItemDelegatePrivate::renderCached(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index, const QtWidgetItemDelegate *delegate) const
{
    // same as above - but use cache to speed-up things
    const QString hashCode = hashIndex(index, option.rect.size());

    QPixmap pixmap;
    if (findPixmap(hashCode, pixmap)) // cache hit
    {
        painter->save();
        painter->translate(option.rect.topLeft());
        painter->drawPixmap(0, 0, pixmap); // draw pixmap from cache
        painter->restore();
    }
    else // cache miss
    {
        delegate->updateWidgetData(index, option); // update widget data

        pixmap = QPixmap(option.rect.size());
        pixmap.fill(Qt::transparent);

        QPainter pixmapPainter(&pixmap);
        widget->render(&pixmapPainter, QPoint(), QRegion(), flags);

        cachePixmap(hashCode, pixmap); // cache pixmap

        painter->drawPixmap(option.rect.topLeft(), pixmap);
    }
}

void QtWidgetItemDelegatePrivate::clearState() const
{
    // clear sub-widget and state
    currentState.control = nullptr;
    currentState.state = QStyle::State_None;
}

void QtWidgetItemDelegatePrivate::resetState() const
{
    // reset sub-widget state, but dont clear state
    if (currentState.control == nullptr || currentState.state == QStyle::State_None)
        return;

    currentState.control->setAttribute(Qt::WA_UnderMouse, false);
    if (qobject_cast<QAbstractButton*>(currentState.control))
        currentState.control->setProperty(isButtonDownProperty, false);
}

void QtWidgetItemDelegatePrivate::applyState() const
{
    // apply sub-widget state
    if (currentState.control == nullptr || currentState.state == QStyle::State_None)
        return;

    currentState.control->setAttribute(Qt::WA_UnderMouse, currentState.state & QStyle::State_MouseOver);
    if ((currentState.state & QStyle::State_Sunken) && qobject_cast<QAbstractButton*>(currentState.control))
        currentState.control->setProperty(isButtonDownProperty, true);
}

void QtWidgetItemDelegatePrivate::resetWidget() const
{
    if (!widget)
        return;

    // reset state of all sub-widgets
    const QList<QWidget*> children = widget->findChildren<QWidget*>();
    for (const auto child : children)
    {
        child->setAttribute(Qt::WA_UnderMouse, false);
        if (auto button = qobject_cast<QAbstractButton*>(child))
            button->setDown(false);
    }
}


QtWidgetItemDelegate::QtWidgetItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , d_ptr(new QtWidgetItemDelegatePrivate)
{
    Q_D(QtWidgetItemDelegate);
    d->cache.setCacheLimit(512);
}

QtWidgetItemDelegate::~QtWidgetItemDelegate()
{
}

void QtWidgetItemDelegate::setAutoFillBackground(bool on)
{
    Q_D(QtWidgetItemDelegate);
    if (on)
        d->flags |= QWidget::DrawWindowBackground;
    else
        d->flags &= ~QWidget::DrawWindowBackground;
}

bool QtWidgetItemDelegate::isAutoFillBackground() const
{
    Q_D(const QtWidgetItemDelegate);
    return (d->flags & QWidget::DrawWindowBackground);
}

void QtWidgetItemDelegate::setStaticContent(bool on)
{
    Q_D(QtWidgetItemDelegate);
    d->staticContents = on;
    if (d->staticContents) // enable caching
        d->cacheEnabled = true;
}

bool QtWidgetItemDelegate::isStaticContent() const
{
    Q_D(const QtWidgetItemDelegate);
    return d->staticContents;
}

void QtWidgetItemDelegate::setCacheEnabled(bool enabled)
{
    Q_D(QtWidgetItemDelegate);
    d->cacheEnabled = enabled;
}

bool QtWidgetItemDelegate::isCacheEnabled() const
{
    Q_D(const QtWidgetItemDelegate);
    return d->cacheEnabled;
}

void QtWidgetItemDelegate::setCacheLimit(int cacheSize)
{
    Q_D(QtWidgetItemDelegate);
    d->cache.setCacheLimit(cacheSize);
}

int QtWidgetItemDelegate::cacheLimit() const
{
    Q_D(const QtWidgetItemDelegate);
    return d->cache.cacheLimit();
}

void QtWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_D(const QtWidgetItemDelegate);
    if (!index.isValid())
        return;

    createWidgetOnDemand(); // lazy widget creation
    if ((d->widget == Q_NULLPTR))
        return QStyledItemDelegate::paint(painter, option, index);

    if (auto view = qobject_cast<const QAbstractItemView*>(option.widget))
    {
        // Workaround for Qt bug (it doesn't set QStyle::State_MouseOver in some circumstances)
        // This situation can occur when some rows/columns was inserted/removed from model
        // but we have not receive any mouse events, so QStyle::State_MouseOver was not updated.
        // Here we determine it using QCursor::pos() and reset option.state accordingly
        if (auto viewport = view->viewport())
        {
            const QPoint localPos = viewport->mapFromGlobal(QCursor::pos());
            if (option.rect.contains(localPos))
                const_cast<QStyleOptionViewItem&>(option).state |= QStyle::State_MouseOver;
            else
                const_cast<QStyleOptionViewItem&>(option).state &= ~QStyle::State_MouseOver;
        }
    }

    // resize widget if needed
    if (d->widget->size() != option.rect.size())
        d->widget->resize(option.rect.size());

    // draw item backdground
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
    else if (option.state & QStyle::State_MouseOver)
        painter->fillRect(option.rect, option.palette.color(QPalette::Highlight).lighter(180));
    else
        painter->fillRect(option.rect, Qt::white);

    if (!d->staticContents) // if we have dynamic content (such as a buttons)
    {
        // we need some workaround like that described above for
        // hadling persistent state of sub-widget
        if ((option.state & QStyle::State_MouseOver) && d->currentIndex != index)
        {
            d->resetState(); // reset widget state
            d->currentIndex = index; // update current index
            d->applyState(); // apply saveed state
        }
        else
        {
            if (d->currentIndex == index)
                d->applyState();
            else
                d->resetState();
        }

    }

    // evaluate how to draw widget - does we need caching
    // or update widget data and draw directly
    const RenderHint hint = renderHint(option, index);
    if (hint == RenderDirect)
    {
        updateWidgetData(index, option);
        d->renderDirect(painter, option.rect);
    }
    else
    {
        d->renderCached(painter, option, index, this);
    }
}

// Hint for QtWidgetItemDelegate::paint() method - does we need
// to render from cache or bypass cache and render directly
// Derived classes can overload this method to provide custom behavior
QtWidgetItemDelegate::RenderHint QtWidgetItemDelegate::renderHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    Q_D(const QtWidgetItemDelegate);
    // if state mouse over or selected we allways draw widget directly (to avoid artefacts)
    // if caching was disabled explicitly - allways draw widget directly
    // in other cases - we try to use cache
    return (((option.state & (QStyle::State_MouseOver|QStyle::State_Selected)) || !d->cacheEnabled) ? RenderHint::RenderDirect : RenderHint::RenderCached);
}

QtItemWidget *QtWidgetItemDelegate::widget() const
{
    Q_D(const QtWidgetItemDelegate);
    return d->widget.get();
}

QSize QtWidgetItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_D(const QtWidgetItemDelegate);
    if (!index.isValid())
        return QStyledItemDelegate::sizeHint(option, index);

    createWidgetOnDemand();
    if (Q_UNLIKELY(d->widget == Q_NULLPTR))
        return QStyledItemDelegate::sizeHint(option, index);

    // use widget minimum size as sizeHint for item
    return d->widget->minimumSize();
}

void QtWidgetItemDelegate::createWidgetOnDemand() const
{
    Q_D(const QtWidgetItemDelegate);
    if (d->widget)
        return;

    d->widget.reset(createItemWidget());
    if (Q_UNLIKELY(d->widget == Q_NULLPTR))
        return;

    // This is a most important thing - set an
    // Qt::WA_DontShowOnScreen attribute on widget
    // to disable widget ability to self-render
    d->widget->setAttribute(Qt::WA_DontShowOnScreen);
    d->widget->setMouseTracking(true);
    // This also important: we adjust widget size beforehand
    // to get correct value for sizeHint() overriden method
    d->widget->adjustSize();
}

void QtWidgetItemDelegate::updateWidgetData(const QModelIndex& index, const QStyleOptionViewItem& option) const
{
    Q_D(const QtWidgetItemDelegate);
    if (d->widget)
        d->widget->setData(index, option); // setup widget from data in index
}


bool QtWidgetItemDelegate::eventHandler(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_D(QtWidgetItemDelegate);
    // this is a single point that implements event handling:
    // no matter from where we got an event - it can be a
    // editorEvent() overriden or eventFilter() overriden method, but not both
    if (!d->widget || !model || !index.isValid())
    {
        if (d->eventFilterEnabled)
            return false;
        else
            return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    QWidget* widget = const_cast<QWidget*>(option.widget);
    QWidget* viewport = nullptr;
    QAbstractItemView* view = qobject_cast<QAbstractItemView*>(widget);
    if (view)
        viewport = view->viewport();

    if (!view || !viewport)
        return QStyledItemDelegate::editorEvent(event, model, option, index);

    if (model->rowCount() == 0 || model->columnCount() == 0)
        view->unsetCursor(); // no rows or no columns, model is empty - reset cursor

    switch (event->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    {
        // update current index and state
        if (d->currentIndex != index)
        {
            d->clearState();
            d->resetState();
            view->update(d->currentIndex);
        }
        d->currentIndex = index;
        d->applyState();

        // move widget (this save us from coping event data)
        d->widget->move(viewport->mapToGlobal(option.rect.topLeft()));

        if (d->staticContents)
            d->widget->viewportEvent(event, widget); // resend event directly
        else
            d->widget->handleEvent(event, widget, d->currentState); // resend event directly, while updating current state

        // force to repaint item rect
        viewport->repaint(option.rect);

        break;
    }
    default:
        break;
    }

    if (d->eventFilterEnabled)
        return false;
    else
        return QStyledItemDelegate::editorEvent(event, model, option, index);
}

bool QtWidgetItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_D(QtWidgetItemDelegate);
    if (d->eventFilterEnabled)
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    else
        return eventHandler(event, model, option, index);
}

bool QtWidgetItemDelegate::eventFilter(QObject *object, QEvent *event)
{
    Q_D(QtWidgetItemDelegate);
    if (!d->eventFilterEnabled)
        return QObject::eventFilter(object, event);

    QWidget* viewport = nullptr;
    QAbstractItemView* view = qobject_cast<QAbstractItemView*>(parent());
    if (view)
        viewport = view->viewport();

    if (!view || !viewport)
        return QObject::eventFilter(object, event);

    QAbstractItemModel* model = view->model();
    if (!model)
        return QObject::eventFilter(object, event);

    if (object == viewport || object == view)
    {
        QModelIndex index = view->indexAt(viewport->mapFromGlobal(QCursor::pos()));
        QStyleOptionViewItem optionView;
        optionView.initFrom(viewport);
        optionView.rect = view->visualRect(index);
        optionView.decorationSize = view->iconSize();
        optionView.widget = view;
        return eventHandler(event, model, optionView, index);
    }
    return QObject::eventFilter(object, event);
}

void QtWidgetItemDelegate::invalidate()
{
    Q_D(QtWidgetItemDelegate);
    d->cache.clear();
    Q_EMIT requestRepaint();
}

void QtWidgetItemDelegate::invalidateIndex(const QModelIndex &index)
{
    Q_D(QtWidgetItemDelegate);
    auto view = qobject_cast<QAbstractItemView*>(parent());
    if (view)
        d->cache.remove(hashIndex(index, view->visualRect(index).size()));
}

void QtWidgetItemDelegate::invalidateRange(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_D(QtWidgetItemDelegate);
    auto view = qobject_cast<QAbstractItemView*>(parent());
    if (!view)
        return;

    const QModelIndex index = topLeft;
    for (int i = topLeft.row(), n = bottomRight.row(); i <= n; ++i)
        for (int j = topLeft.column(), m = bottomRight.column(); j <= m; ++j)
            d->cache.remove(hashIndex(index.sibling(i, j), view->visualRect(index).size()));
}


QtItemWidget::QtItemWidget(QWidget *parent) : QWidget(parent)
{
}

QtItemWidget::~QtItemWidget()
{
}

void QtItemWidget::setData(const QModelIndex&, const QStyleOptionViewItem &)
{
    // does nothing by default
}

bool QtItemWidget::viewportEvent(QEvent *e, QWidget*)
{
    return QWidget::event(e);
}

bool QtItemWidget::isMouseOver(QWidget *w, QMouseEvent *e) const
{
    return w->rect().contains(w->mapFromGlobal(e->globalPos()));
}

bool QtItemWidget::handleEvent(QEvent *e, QWidget *w, WidgetState &wstate)
{
    // this is an important part: here we
    // handle an event and update sub-widget state
    if (e->type() == QEvent::MouseMove)
    {
        wstate.control = nullptr;
        wstate.state &= ~QStyle::State_MouseOver;
        QCursor cursor(Qt::ArrowCursor);
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
        const QList<QWidget*> children = findChildren<QWidget*>();
        for (auto child : children)
        {
            const bool underMouse = isMouseOver(child, mouseEvent);
            child->setAttribute(Qt::WA_UnderMouse, underMouse);
            if (underMouse)
            {
                wstate.control = child;
                wstate.state |= QStyle::State_MouseOver;
                if (child->testAttribute(Qt::WA_SetCursor) && child->isEnabled())
                    cursor = child->cursor();
            }
        }

        w->setCursor(cursor);
    }

    if (e->type() == QEvent::MouseButtonPress)
    {
        wstate.state &= ~QStyle::State_Sunken;
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
        const QList<QAbstractButton*> buttons = findChildren<QAbstractButton*>();
        for (auto button : buttons)
        {
            button->setDown(false);
            if (button == wstate.control)
            {
                if (button->isEnabled() && isMouseOver(button, mouseEvent))
                    wstate.state |= QStyle::State_Sunken;
                else
                    wstate.state &= ~QStyle::State_Sunken;
                button->setDown(wstate.state & QStyle::State_Sunken);
            }
        }
    }

    if (e->type() == QEvent::MouseButtonRelease)
    {
        wstate.state &= ~QStyle::State_Sunken;
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
        const QList<QAbstractButton*> buttons = findChildren<QAbstractButton*>();
        for (auto button : buttons)
        {
            if (button == wstate.control && button->isDown() && button->isEnabled())
                if ((wstate.state & QStyle::State_MouseOver) || isMouseOver(button, mouseEvent))
                    button->click();

            button->setDown(false);
        }
    }
    // send event to virtual method, so derived classes can
    // can implement their own custom event handling
    return viewportEvent(e, w);
}
