#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QMouseEvent>

#include <QDebug>

#include "qtcolorgrid.h"
#include "qtcolorgrid_p.h"


QtColorGridModel::QtColorGridModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

QtColorGridModel::~QtColorGridModel()
{
}

void QtColorGridModel::setPalette(const QtColorPalette &palette)
{
    if (!mPalette.isEmpty()) {
        if (palette.size() == mPalette.size()) {
            mPalette = palette;
            emit dataChanged(index(0), index(rowCount() - 1));
            return;
        }
        clear();
    }

    if (!palette.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, palette.size() - 1);
        mPalette = palette;
        endInsertRows();
    }
}

const QtColorPalette &QtColorGridModel::palette()
{
    return mPalette;
}

int QtColorGridModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return mPalette.size();
}

QVariant QtColorGridModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= mPalette.size()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DecorationRole:
        return mPalette.at(row);
    case Qt::EditRole:
        return mPalette.at(row);
    case Qt::ToolTipRole:
        return mPalette.at(row).name();
    default:
        break;
    }
    return QVariant();
}

void QtColorGridModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, mPalette.size() - 1);
    mPalette.clear();
    endRemoveRows();
}





QtColorGridDelegate::QtColorGridDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QtColorGridDelegate::~QtColorGridDelegate()
{
}

void QtColorGridDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();

    opt.showDecorationSelected = false;
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect.adjusted(0, 0, 1, 1), Qt::red);
        opt.state ^= QStyle::State_Selected;
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
        return;
    }

    if (opt.state & QStyle::State_MouseOver)
    {
        const QPalette& pal = widget ? widget->palette() : QApplication::palette();
        painter->fillRect(opt.rect.adjusted(1, 1, 0, 0), pal.color(QPalette::Highlight));
        opt.rect.adjust(2, 1, -1, -1);
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
        return;
    }

    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}






class QtColorGridPrivate
{
public:
    QScopedPointer<QtColorGridModel> model;
    QtColorGridPrivate(QtColorGrid* q) :
        model(new QtColorGridModel(q)) {
    }
};


QtColorGrid::QtColorGrid(QWidget *parent) :
    QListView(parent), d_ptr(new QtColorGridPrivate(this))
{
    Q_D(QtColorGrid);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    resizeIcons(QSize(16, 16));
    setViewMode(QListView::IconMode);
    setWrapping(true);
    setResizeMode(QListView::Adjust);
    setUniformItemSizes(true);
    setItemDelegate(new QtColorGridDelegate(this));
    setMouseTracking(true);
    setModel(d->model.data());
    connect(this, SIGNAL(iconSizeChanged(QSize)), SLOT(resizeIcons(QSize)));
}

QtColorGrid::~QtColorGrid()
{
}

void QtColorGrid::setColors(const QVector<QColor> &colors)
{
    QtColorPalette tmp;
    for (auto it = colors.begin(); it != colors.end(); ++it) {
        tmp.insert(*it);
    }
    setColors(tmp);
}

void QtColorGrid::setColors(const QList<QColor> &colors)
{
    QtColorPalette tmp;
    for (auto it = colors.begin(); it != colors.end(); ++it) {
        tmp.insert(*it);
    }
    setColors(tmp);
}

void QtColorGrid::setColors(const QtColorPalette &palette)
{
    Q_D(QtColorGrid);
    if (d->model->palette() != palette) {
        d->model->setPalette(palette);
        setMinimumSize(gridSize().width() + 3, gridSize().height() + 3);
        setMaximumWidth(gridSize().width() * palette.size() + 3);
        setMaximumHeight(gridSize().height() * palette.size() + 3);
        emit colorsChanged();
    }
}

const QtColorPalette &QtColorGrid::colors() const
{
    Q_D(const QtColorGrid);
    return d->model->palette();
}

QColor QtColorGrid::currentColor() const
{
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        return index.data(Qt::EditRole).value<QColor>();
    }
    return QColor();
}

void QtColorGrid::clear()
{
    Q_D(QtColorGrid);
    d->model->clear();
}

void QtColorGrid::resizeIcons(const QSize &size)
{
    Q_D(QtColorGrid);
    blockSignals(true);
    QtColorPalette palette = d->model->palette();
    setIconSize(QSize(size.width()-1, size.height()));
    setGridSize(size);
    setMinimumSize(size.width() + 3, size.height() + 3);
    setMaximumWidth(size.width() * palette.size() + 3);
    setMaximumHeight(size.height() * palette.size() + 3);
    blockSignals(false);
}

void QtColorGrid::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
}

void QtColorGrid::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);
    if (current.isValid())
        Q_EMIT colorChanged(current.data(Qt::EditRole).value<QColor>());
    doItemsLayout();
}


void QtColorGrid::mouseMoveEvent(QMouseEvent *event)
{
    QListView::mouseMoveEvent(event);

    if (!hasMouseTracking())
        return;

    QModelIndex index = indexAt(viewport()->mapFromGlobal(event->globalPos()));
    if (!index.isValid())
        return;

    QModelIndex current = currentIndex();
    if (!current.isValid())
        return;

    QRect r1 = visualRect(current).adjusted(0, 0, 2, 2);
    QRect r2 = visualRect(index);
    if (r1.right() >= r2.left() || r1.bottom() >= r2.top())
        doItemsLayout();

}

