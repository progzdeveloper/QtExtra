#include "qtitemviewnavigator.h"
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QPointer>

class QtItemViewNavigatorPrivate
{
public:
    QPointer<QAbstractItemView> view;
};

QtItemViewNavigator::QtItemViewNavigator(QWidget *parent) :
    QtRangeNavigator(parent), d_ptr(new QtItemViewNavigatorPrivate)
{
}

QtItemViewNavigator::QtItemViewNavigator(QAbstractItemView *view, QWidget *parent) :
    QtRangeNavigator(parent), d_ptr(new QtItemViewNavigatorPrivate)
{
    setView(view);
}

QtItemViewNavigator::~QtItemViewNavigator()
{
}

void QtItemViewNavigator::setView(QAbstractItemView *view)
{
    Q_D(QtItemViewNavigator);

    if (view == Q_NULLPTR)
        return;

    QAbstractItemModel* model = view->model();
    if (model == Q_NULLPTR)
        return;

    d->view = view;
    setRange(0, model->rowCount());
    connect(this, SIGNAL(indexChanged(int)), SLOT(moveTo(int)));
    connect(view, SIGNAL(clicked(const QModelIndex&)), SLOT(setIndex(const QModelIndex&)));
    connect(view->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)), SLOT(updateRange()));
    connect(view->model(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)), SLOT(updateRange()));
}

QAbstractItemView *QtItemViewNavigator::view() const
{
    Q_D(const QtItemViewNavigator);
    return d->view;
}

void QtItemViewNavigator::moveTo(int row)
{
    Q_D(QtItemViewNavigator);
    QAbstractItemModel* model = d->view->model();
    QModelIndex index = model->index(row, 0);
    if (!index.isValid()) {
        return;
    }
    d->view->scrollTo(index);
    QItemSelectionModel* selectionModel = d->view->selectionModel();
    if (selectionModel != Q_NULLPTR)
        selectionModel->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
}

void QtItemViewNavigator::setIndex(const QModelIndex &index)
{
    if (index.isValid()) {
        blockSignals(true);
        setCurrent(index.row() + 1);
        blockSignals(false);
    }
}

void QtItemViewNavigator::updateRange()
{
    Q_D(QtItemViewNavigator);
    if (d->view == Q_NULLPTR) {
        setRange(0, 0);
        return;
    }

    QAbstractItemModel* model = d->view->model();
    if (model == Q_NULLPTR) {
        setRange(0, 0);
        return;
    }

    setMaximum(model->rowCount());
}
