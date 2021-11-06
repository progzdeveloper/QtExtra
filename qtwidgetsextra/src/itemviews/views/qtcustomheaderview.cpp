#include "qtcustomheaderview.h"
#include <QApplication>
#include <QStyle>
#include <QMenu>
#include <QAction>

#include <QHoverEvent>
#include <QPainter>
#include <QPolygon>
#include <QCursor>
#include <QIcon>

#include <QToolTip>


class QtCustomHeaderViewPrivate
{
public:
    QHash<int, QIcon> statusIcons;
    QHash<int, quint32> statusMap;
    QtCustomHeaderView *q_ptr;
    QAction *collapse;
    QAction *sortAsc;
    QAction *sortDesc;
    QIcon menuIcon;
    bool sortingEnabled;

    QRect sectionRect(int logicalIndex) const;
    QRect buttonMenuRect(int logicalIndex) const;
    QRect checkStateRect(int logicalIndex) const;
    QRect prevRect(int logicalIndex) const;
    QRect nextRect(int logicalIndex) const;

    void setMenuIcon();
    void drawSectionIcon(QPainter *painter, int logicalIndex, bool enabled) const;
    void drawPrevButton(QPainter *painter, int logicalIndex, bool enabled) const;
    void drawNextButton(QPainter *painter, int logicalIndex, bool enabled) const;
    void drawArrow(QPainter* painter, const QRectF& r, int arrowType) const;
    void drawCheckBox(QPainter* painter, const QRect& rect, Qt::CheckState state) const;
};

void QtCustomHeaderViewPrivate::setMenuIcon()
{
#ifndef QXTHEADERVIEW_NO_ICONS
    // cache menu icon
    menuIcon = QIcon(":/images/expand");
#endif
}

QRect QtCustomHeaderViewPrivate::sectionRect(int logicalIndex) const
{
    switch(q_ptr->orientation()) {
    case Qt::Horizontal:
        return QRect(q_ptr->sectionViewportPosition(logicalIndex), 0, q_ptr->sectionSize(logicalIndex), q_ptr->height());
    case Qt::Vertical:
        return QRect(0, q_ptr->sectionViewportPosition(logicalIndex), q_ptr->width(), q_ptr->sectionSize(logicalIndex));
    }
    return QRect();
}

QRect QtCustomHeaderViewPrivate::buttonMenuRect(int logicalIndex) const
{
    QRect sr = sectionRect(logicalIndex);
    bool isNextHidden = q_ptr->isSectionHidden(logicalIndex + 1);
    switch(q_ptr->orientation()) {
    case Qt::Horizontal:
        return QRect(sr.right() - (isNextHidden ? 30 : 25), sr.center().y() - 8, 16, 16);
    case Qt::Vertical:
        return QRect(sr.right() - 25, sr.center().y() - 8, 16, 16);
    }
    return QRect();
}

QRect QtCustomHeaderViewPrivate::checkStateRect(int logicalIndex) const
{
#if 0
    QRect sr = sectionRect(logicalIndex);
    switch (q_ptr->orientation()) {
    case Qt::Horizontal:
        return QRect(sr.left(), sr.top(), sr.height(), sr.height());
    case Qt::Vertical:
        return QRect(sr.left(), sr.top(), sr.height(), sr.height());
    }
    return QRect();
#endif
    QStyle* style = q_ptr->style();
    QStyleOptionButton opt;
    opt.initFrom(q_ptr);
    QRectF r = style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt, q_ptr);

    switch(q_ptr->orientation()) {
        case Qt::Horizontal: {
            qreal margin = (q_ptr->height() - r.height()) / 2;
            qreal offset = q_ptr->isSectionHidden(logicalIndex - 1) ? (q_ptr->height() / 2 + 4) : 0;
            r.moveTopLeft(QPoint(q_ptr->sectionViewportPosition(logicalIndex) + margin + offset, margin));
        }
        break;
        case Qt::Vertical: {
            QRect sr = sectionRect(logicalIndex);
            qreal margin = (q_ptr->width() - r.width()) / 2;
            qreal offset = sr.height() / 2 - r.width() / 2 + 1;
            r.moveTopLeft(QPoint(margin, q_ptr->sectionViewportPosition(logicalIndex) + offset));
        }
        break;
    }
    return r.toRect();
}


QRect QtCustomHeaderViewPrivate::prevRect(int logicalIndex) const
{
    if (q_ptr->isSectionHidden(logicalIndex))
        return QRect();

    QRect sr = sectionRect(logicalIndex);
    //return QRect(sr.left() + 1, sr.center().y() - 6, 13, 13);

    int x = 0, y = 0, w = 0;
    switch(q_ptr->orientation()) {
        case Qt::Horizontal:
            w = sr.height() / 2 - 1;
            x = sr.left() + 1;
            y = sr.top() + w - 3;
            break;
        case Qt::Vertical:
            w = 8; // sr.height() / 2 - 1;
            x = sr.right() - (w + 2);
            y = sr.top() + 1;
            break;
    }
    return QRect(x, y, w, w);
}

QRect QtCustomHeaderViewPrivate::nextRect(int logicalIndex) const
{
    if (q_ptr->isSectionHidden(logicalIndex))
        return QRect();

    QRect sr = sectionRect(logicalIndex);
    //return QRect(sr.right() - 13, sr.center().y() - 6, 13, 13);
    int x = 0, y = 0, w = 0;
    switch(q_ptr->orientation()) {
        case Qt::Horizontal:
            w = sr.height() / 2 - 1;
            x = sr.right() - (w + 1);
            y = sr.top() + w - 3;
            break;
        case Qt::Vertical:
            w = 8;
            x = sr.right() - (w + 2);
            y = sr.bottom() - (w + 1);
            break;
    }
    return QRect(x, y, w, w);
}

void QtCustomHeaderViewPrivate::drawSectionIcon(QPainter *painter, int logicalIndex, bool /*enabled*/) const
{
#ifndef QXTHEADERVIEW_NO_ICONS
    //menuIcon.paint(painter, buttonMenuRect(logicalIndex), Qt::AlignCenter, enabled ? QIcon::Normal : QIcon::Disabled, QIcon::On);
    auto it = statusMap.find(logicalIndex);
    if (it != statusMap.end()) {
        auto iconIt = statusIcons.find(*it);
        if (iconIt != statusIcons.end()) {
            //iconIt->paint(painter, buttonMenuRect(logicalIndex), Qt::AlignCenter, enabled ? QIcon::Normal : QIcon::Disabled, QIcon::On);
            iconIt->paint(painter, buttonMenuRect(logicalIndex), Qt::AlignCenter, QIcon::Normal, QIcon::On);
        }
    }
#else
    QRectF brect(buttonMenuRect(logicalIndex));
    painter->setPen(enabled ? QColor(186,186,186) : QColor(223, 223, 223));
    painter->setBrush(QColor(246,246,246));
    painter->drawRoundedRect(brect.adjusted(0,0,-1,-1), 3, 3);

    painter->setPen(enabled ? QColor(71,71,71) : QColor(193, 193, 193));
    painter->setBrush(enabled ? QColor(71,71,71) : QColor(193, 193, 193));
    drawArrow(painter, brect.adjusted(3, 3, -3, -3), ArrowDown);
#endif

}

void QtCustomHeaderViewPrivate::drawPrevButton(QPainter *painter, int logicalIndex, bool enabled) const
{
    QRect rect = prevRect(logicalIndex);

    painter->setPen(enabled ? QColor(71,71,71) : QColor(186,186,186));
    painter->setBrush(enabled ? QColor(71,71,71) : QColor(186,186,186));
    switch(q_ptr->orientation())
    {
    case Qt::Horizontal:
        drawArrow(painter, rect, Qt::LeftArrow);
        break;
    case Qt::Vertical:
        drawArrow(painter, rect, Qt::UpArrow);
        break;
    }
}

void QtCustomHeaderViewPrivate::drawNextButton(QPainter *painter, int logicalIndex, bool enabled) const
{
    QRect rect = nextRect(logicalIndex);

    painter->setPen(enabled ? QColor(71,71,71) : QColor(186,186,186));
    painter->setBrush(enabled ? QColor(71,71,71) : QColor(186,186,186));
    switch(q_ptr->orientation())
    {
    case Qt::Horizontal:
        drawArrow(painter, rect, Qt::RightArrow);
        break;
    case Qt::Vertical:
        drawArrow(painter, rect, Qt::DownArrow);
        break;
    }
}

void QtCustomHeaderViewPrivate::drawArrow(QPainter* painter, const QRectF& r, int arrowType) const
{
    QPolygonF arrow;
    switch(arrowType)
    {
    case Qt::UpArrow:
        arrow << r.bottomLeft() << QPointF(r.center().x(), r.top()) << r.bottomRight();
        break;
    case Qt::DownArrow:
        arrow << r.topLeft() << QPointF(r.center().x(), r.bottom()) << r.topRight();
        break;
    case Qt::LeftArrow:
        arrow << r.topRight() << QPointF(r.left(), r.center().y()) << r.bottomRight();
        break;
    case Qt::RightArrow:
        arrow << r.topLeft() << QPointF(r.right(), r.center().y()) << r.bottomLeft();
        break;
    }
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPolygon(arrow);
    painter->setRenderHint(QPainter::Antialiasing, false);
}

void QtCustomHeaderViewPrivate::drawCheckBox(QPainter * painter, const QRect& rect, Qt::CheckState state) const
{
    QStyleOptionViewItem opt;
    opt.initFrom(q_ptr);
    opt.rect = rect;
    opt.state = opt.state & ~QStyle::State_HasFocus;

    switch (state) {
    case Qt::Unchecked:
        opt.state |= QStyle::State_Off;
        break;
    case Qt::PartiallyChecked:
        opt.state |= QStyle::State_NoChange;
        break;
    case Qt::Checked:
        opt.state |= QStyle::State_On;
        break;
    }
    QStyle *style = q_ptr->style();
    if (!style)
        style = QApplication::style();
    style->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter, q_ptr);
}






QtCustomHeaderView::QtCustomHeaderView(Qt::Orientation orientation, QWidget * parent /*= 0*/)
    : QHeaderView(orientation, parent), d_ptr(new QtCustomHeaderViewPrivate)
{
    d_ptr->q_ptr = this;
    //d_ptr->setMenuIcon();
    // Required to refresh button menu when the mouse leave the header.
    setAttribute(Qt::WA_Hover, true);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    setSectionsClickable(true);
#else
    setClickable(true);
#endif
}


QtCustomHeaderView::~QtCustomHeaderView(void)
{
}


void QtCustomHeaderView::setSortingEnabled(bool on)
{
    Q_D(QtCustomHeaderView);
    d->sortingEnabled = on;
}

bool QtCustomHeaderView::isSortingEnabled() const
{
    Q_D(const QtCustomHeaderView);
    return d->sortingEnabled;
}

void QtCustomHeaderView::setSectionStatus(int section, quint32 status)
{
    Q_D(QtCustomHeaderView);
    d->statusMap[section] = status;
    updateSection(section);
}

quint32 QtCustomHeaderView::sectionStatus(int section) const
{
    Q_D(const QtCustomHeaderView);
    return d->statusMap[section];
}

void QtCustomHeaderView::setStatusIcon(int status, const QIcon &icon)
{
    Q_D(QtCustomHeaderView);
    d->statusIcons[status] = icon;
}

QIcon QtCustomHeaderView::statusIcon(int status) const
{
    Q_D(const QtCustomHeaderView);
    return d->statusIcons[status];
}

QMenu *QtCustomHeaderView::createStandardContextMenu()
{
    Q_D(QtCustomHeaderView);
    QMenu *menu = new QMenu(this);
    switch(orientation()) {
    case Qt::Horizontal:
        d->collapse = menu->addAction(tr("Hide column"));
        if (d->sortingEnabled) {
#ifndef QCUSTOMHEADERVIEW_NO_ICONS
            d->sortAsc  = menu->addAction(QIcon::fromTheme("view-sort-ascending", QIcon(":/images/sort-asc")), tr("Sort ascending"));
            d->sortDesc = menu->addAction(QIcon::fromTheme("view-sort-descending", QIcon(":/images/sort-desc")), tr("Sort descending"));
#else
            d->sortAsc  = menu->addAction(tr("Sort ascending"));
            d->sortDesc = menu->addAction(tr("Sort descending"));
#endif
        }
        break;
    case Qt::Vertical:
        d->collapse = menu->addAction(tr("Hide row"));
        break;
    }

    // Disable hide column if only one column remains. Otherwise
    // the gui is no more available to show them back.
    d->collapse->setEnabled(hiddenSectionCount() < count() - 1);
    return menu;
}

void QtCustomHeaderView::mousePressEvent(QMouseEvent * event)
{
    Q_D(QtCustomHeaderView);

    int logicalIndex = logicalIndexAt(event->pos());

    // ###
    if (event->button() == Qt::RightButton)
    {
        if (contextMenuPolicy() == Qt::ActionsContextMenu) {
            QContextMenuEvent e(QContextMenuEvent::Other, event->pos());
            contextMenuEvent(&e);
        }
        event->ignore();
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        if (d->buttonMenuRect(logicalIndex).contains(event->pos())) {
            QContextMenuEvent e(QContextMenuEvent::Other, event->pos());
            contextMenuEvent(&e);
            event->ignore();
            return;
        }

        if (d->checkStateRect(logicalIndex).contains(event->pos())) {
            QVariant stateValue = model()->headerData(logicalIndex, orientation(), Qt::CheckStateRole);
            if (stateValue.isValid()) {
                Qt::CheckState checkState = static_cast<Qt::CheckState>(stateValue.toInt());
                checkState = (checkState == Qt::Unchecked || checkState == Qt::PartiallyChecked ? Qt::Checked : Qt::Unchecked);
                model()->setHeaderData(logicalIndex, orientation(), checkState, Qt::CheckStateRole);
                event->ignore();
            }
            return;
        }

        emit sectionClicked(logicalIndex);

        // Catch previous arrow mouse click.
        if (d->prevRect(logicalIndex).contains(event->pos())) {
            showSection(logicalIndex - 1);
            updateSection(logicalIndex - 2);
            event->ignore();
            return;
        }

        // Catch next arrow mouse click.
        if (d->nextRect(logicalIndex).contains(event->pos())) {
            showSection(logicalIndex + 1);
            updateSection(logicalIndex + 2);
            event->ignore();
            return;
        }
    }

    QHeaderView::mousePressEvent(event);
}

void QtCustomHeaderView::mouseMoveEvent(QMouseEvent * event)
{
    Q_D(QtCustomHeaderView);

    QHeaderView::mouseMoveEvent(event);

    // Required to refresh the button menu enable/disable state.
    int logicalIndex = logicalIndexAt(event->pos());
    updateSection(logicalIndex);

    QString name;
    if (d->prevRect(logicalIndex).contains(event->pos()) &&
        isSectionHidden(logicalIndex - 1)) {
        name = model()->headerData(logicalIndex - 1, orientation()).toString();
    }

    if (d->nextRect(logicalIndex).contains(event->pos()) &&
        isSectionHidden(logicalIndex + 1)) {
        name = model()->headerData(logicalIndex + 1, orientation()).toString();
    }

    if (!name.isEmpty()) {
        QPoint pos = mapToGlobal(event->pos());
        switch(orientation()) {
        case Qt::Horizontal:
            QToolTip::showText(pos, tr("Show %1 column").arg(name));
            break;
        case Qt::Vertical:
            QToolTip::showText(pos, tr("Show %1 row").arg(name));
            break;
        }
    }

    repaint();
}

void QtCustomHeaderView::contextMenuEvent(QContextMenuEvent * event)
{
    Q_D(QtCustomHeaderView);
    QMenu *contextMenu = Q_NULLPTR;
    int logicalIndex = logicalIndexAt(event->pos());
    switch (contextMenuPolicy())
    {
        case Qt::DefaultContextMenu:
        {
            contextMenu = createStandardContextMenu();
            QAction *res = contextMenu->exec(mapToGlobal(event->pos()));
            if (res == d->collapse)
                collapseSection(logicalIndex);
            if (res == d->sortAsc)
                sortModel(logicalIndex, Qt::AscendingOrder);
            if (res == d->sortDesc)
                sortModel(logicalIndex, Qt::DescendingOrder);
            break;
        }
        case Qt::ActionsContextMenu:
        {
            contextMenu = new QMenu(this);
            contextMenu->addActions(this->actions());
            contextMenu->exec(mapToGlobal(event->pos()));
            break;
        }
        case Qt::CustomContextMenu:
        {
            emit customContextMenuRequested(event->pos());
        }
        default:
            break;
    }
    if (contextMenu)
        delete contextMenu;
    updateSection(logicalIndex);
}


void QtCustomHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    Q_D(const QtCustomHeaderView);
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if (!rect.isValid() || isSortIndicatorShown())
        return;

    QVariant stateValue = model()->headerData(logicalIndex, orientation(), Qt::CheckStateRole);
    if (stateValue.isValid()) {
        d->drawCheckBox(painter, d->checkStateRect(logicalIndex), static_cast<Qt::CheckState>(stateValue.toInt()));
    }

    QPoint pos = mapFromGlobal(QCursor::pos());
    if (isSectionHidden(logicalIndex - 1)) {
        d->drawPrevButton(painter, logicalIndex, d->prevRect(logicalIndex).contains(pos));
    }

    if (isSectionHidden(logicalIndex + 1)) {
        d->drawNextButton(painter, logicalIndex, d->nextRect(logicalIndex).contains(pos));
    }

    d->drawSectionIcon(painter, logicalIndex, d->buttonMenuRect(logicalIndex).contains(pos));
    //if (rect.contains(pos)) {
    //    d->drawMenuIcon(painter, logicalIndex, d->buttonMenuRect(logicalIndex).contains(pos));
    //}
}

QSize QtCustomHeaderView::sectionSizeFromContents ( int logicalIndex ) const
{
    //Q_D(const QCustomHeaderView);
    QSize s = QHeaderView::sectionSizeFromContents(logicalIndex);
    switch (orientation()) {
        case Qt::Horizontal:
            s.rwidth() += height() / 2 + 24;
            break;
        case Qt::Vertical:
            s.rwidth() += 32;
            s.setHeight(qMax(16, s.height()));
            break;
    }
    return s;
    //return QHeaderView::sectionSizeFromContents(logicalIndex);
}

void QtCustomHeaderView::sortModel(int column, Qt::SortOrder order)
{
    Q_ASSERT(model() != Q_NULLPTR);
    model()->sort(column, order);
}

void QtCustomHeaderView::collapseSection(int logicalIndex)
{
    hideSection(logicalIndex);
    updateSection(logicalIndex-1);
}

