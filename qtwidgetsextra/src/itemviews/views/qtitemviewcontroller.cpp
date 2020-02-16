#include <QApplication>
#include <QAction>
#include <QActionGroup>

#include <QModelIndex>
#include <QAbstractItemView>
#include <QAbstractItemModel>

#include "qtitemviewcontroller.h"
#include "../delegates/qtactionitemdelegate.h"


class QtItemViewControllerPrivate
{
public:
    static Q_CONSTEXPR int MAX_ROLE = 8;
    QAction* actionMap[8];
    QtItemViewController* controller;
    QAbstractItemView* itemView;
    QActionGroup* actionGroup;
    QtItemViewController::ActionRoles flags;

    QtItemViewControllerPrivate(QtItemViewController* q, QAbstractItemView* view) :
        controller(q),
        itemView(view),
        actionGroup(Q_NULLPTR),
        flags(0)
    {
        memset(actionMap, 0, sizeof(actionMap));
        actionGroup = new QActionGroup(view);
        QObject::connect(actionGroup, SIGNAL(triggered(QAction*)),
                         controller, SIGNAL(triggered(QAction*)));
    }
};


QtItemViewController::QtItemViewController(QAbstractItemView *view, QObject *parent) :
    QObject(parent), d_ptr(new QtItemViewControllerPrivate(this, view))
{
}

QtItemViewController::QtItemViewController(QAbstractItemView* view, ActionRoles actions, QObject *parent) :
    QObject(parent), d_ptr(new QtItemViewControllerPrivate(this, view))
{
    setRoles(actions);
}

QtItemViewController::~QtItemViewController()
{
}

void QtItemViewController::setRoles(ActionRoles actFlags)
{
    Q_D(QtItemViewController);
    clear();

    d->flags = actFlags;
    QStyle* style = qApp->style();
    QAction* act = Q_NULLPTR;

    for (int i = 0, k = 1; i < QtItemViewControllerPrivate::MAX_ROLE; k <<= 1, ++i)
    {
        if ( !d->flags.testFlag(static_cast<ActionRole>(k)) )
            continue;

        switch(k)
        {
        case CommitRole:
            act = new QAction(style->standardIcon(QStyle::SP_DialogApplyButton), tr("Commit"), d->itemView);
            act->setData((int)CommitRole);
            //connect(act, SIGNAL(triggered()), SLOT(commitItem()));
            d->actionMap[i] = act;
            break;
        case RevertRole:
            act = new QAction(style->standardIcon(QStyle::SP_DialogResetButton), tr("Revert"), d->itemView);
            act->setData((int)RevertRole);
            //connect(act, SIGNAL(triggered()), SLOT(revertItem()));
            d->actionMap[i] = act;
            break;

        case InsertRole:
            act = new QAction(QIcon::fromTheme("list-add", QIcon(":/images/plus")), tr("Insert Item"), d->itemView);
            act->setShortcut(Qt::SHIFT + Qt::Key_N);
            act->setData((int)InsertRole);
            //connect(act, SIGNAL(triggered()), SLOT(insertItem()));
            d->actionMap[i] = act;
            break;
        case RemoveRole:
            act = new QAction(QIcon::fromTheme("list-remove", QIcon(":/images/minus")), tr("Remove Item"), d->itemView);
            act->setShortcut(Qt::SHIFT + Qt::Key_Delete);
            act->setData((int)RemoveRole);
            //connect(act, SIGNAL(triggered()), SLOT(removeItem()));
            d->actionMap[i] = act;
            break;

        case MoveUpRole:
            act = new QAction(style->standardIcon(QStyle::SP_ArrowUp), tr("Move Up"), d->itemView);
            act->setShortcut(Qt::ALT + Qt::Key_Up);
            act->setData((int)MoveUpRole);
            //connect(act, SIGNAL(triggered()), SLOT(moveItemUp()));
            d->actionMap[i] = act;
            break;
        case MoveDownRole:
            act = new QAction(style->standardIcon(QStyle::SP_ArrowDown), tr("Move Down"), d->itemView);
            act->setShortcut(Qt::ALT + Qt::Key_Down);
            act->setData((int)MoveDownRole);
            //connect(act, SIGNAL(triggered()), SLOT(moveItemDown()));
            d->actionMap[i] = act;
            break;

        case SortAscRole:
            act = new QAction(QIcon::fromTheme("view-sort-ascending"), tr("Sort Ascending"), d->itemView);
            act->setData((int)SortAscRole);
            //connect(act, SIGNAL(triggered()), SLOT(sortItemsAsc()));
            d->actionMap[i] = act;
            break;
        case SortDescRole:
            act = new QAction(QIcon::fromTheme("view-sort-descending"), tr("Sort Descending"), d->itemView);
            act->setData((int)SortDescRole);
            //connect(act, SIGNAL(triggered()), SLOT(sortItemsDesc()));
            d->actionMap[i] = act;
            break;
        }
        d->actionGroup->addAction(act);
    }
    //enableActions();
}

QtItemViewController::ActionRoles QtItemViewController::roles() const
{
    Q_D(const QtItemViewController);
    return d->flags;
}

void QtItemViewController::addAction(QAction* action)
{
    Q_D(QtItemViewController);
    d->actionGroup->addAction(action);
}

void QtItemViewController::addActions(const QList<QAction*>& actions)
{
    Q_D(QtItemViewController);
    for (auto it = actions.begin(); it != actions.end(); ++it)
        d->actionGroup->addAction(*it);
}

QtItemViewController::ActionRole QtItemViewController::role(QAction *action) const
{
    return static_cast<ActionRole>(action->data().toInt());
}

QAction* QtItemViewController::action(ActionRole witch) const
{
    Q_D(const QtItemViewController);
    int k = witch;
    int i = 0;
    while(k >>= 1)
        ++i;
    if (i < 0 || i >= QtItemViewControllerPrivate::MAX_ROLE)
        return Q_NULLPTR;
    return d->actionMap[i];
}

QList<QAction*> QtItemViewController::actions() const
{
    Q_D(const QtItemViewController);
    return d->actionGroup->actions();
}

void QtItemViewController::clear()
{
    Q_D(QtItemViewController);
    for (auto it = d->actionMap; it != (d->actionMap + QtItemViewControllerPrivate::MAX_ROLE); ++it)
    {
        QAction* a = *it;
        d->actionGroup->removeAction(a);
        if (a)
            delete a;
        a = Q_NULLPTR;
    }
    d->flags = 0;
}

QtActionItemDelegate* QtItemViewController::createDelegate(QObject *parent) const
{
    Q_D(const QtItemViewController);
    QtActionItemDelegate* delegate = new QtActionItemDelegate(parent);
    delegate->addActions(d->actionGroup->actions());
    return delegate;
}

void QtItemViewController::insertItem()
{
    Q_D(QtItemViewController);
    QAbstractItemModel* model = d->itemView->model();
    if (!model)
        return;

    int row = model->rowCount();
    QModelIndex index = d->itemView->currentIndex();
    if (index.isValid())
        row = index.row()+1;
    model->insertRow(row);
    enableActions();
}

void QtItemViewController::removeItem()
{
    Q_D(QtItemViewController);
    QAbstractItemModel* model = d->itemView->model();
    if (!model)
        return;

    QModelIndex index = d->itemView->currentIndex();
    if (index.isValid()) {
        d->itemView->closePersistentEditor(index);
        model->removeRow(index.row());
    }
    enableActions();
}

void QtItemViewController::moveItemUp()
{
    Q_D(QtItemViewController);
    QAbstractItemModel* model = d->itemView->model();
    if (!model)
        return;

    QModelIndex index = d->itemView->currentIndex();
    if (!index.isValid())
        return;

    int row = index.row();
    if (row == 0)
        return;

    int prev = row - 1;
    model->moveRow(index.parent(), row,
                   model->index(prev, index.column()).parent(), prev);

}

void QtItemViewController::moveItemDown()
{
    Q_D(QtItemViewController);
    QAbstractItemModel* model = d->itemView->model();
    if (!model)
        return;

    QModelIndex index = d->itemView->currentIndex();
    if (!index.isValid())
        return;

    int row = index.row();
    if (row == model->rowCount()-1)
        return;

    int next = row + 2;
    model->moveRow(index.parent(), row,
                   model->index(next, index.column()).parent(), next);
}

void QtItemViewController::editItem()
{
    Q_D(QtItemViewController);
    d->itemView->openPersistentEditor(d->itemView->currentIndex());
}

void QtItemViewController::sortAsc()
{
    Q_D(QtItemViewController);
    QModelIndex index = d->itemView->currentIndex();
    QAbstractItemModel* model = d->itemView->model();
    if (!model)
        return;

    model->sort(index.column(), Qt::AscendingOrder);
}

void QtItemViewController::sortDesc()
{
    Q_D(QtItemViewController);
    QModelIndex index = d->itemView->currentIndex();
    QAbstractItemModel* model = d->itemView->model();
    if (!model)
        return;

    model->sort(index.column(), Qt::DescendingOrder);
}

void QtItemViewController::commit()
{
    Q_D(QtItemViewController);
    QAbstractItemModel* model = d->itemView->model();
    if (!model)
        return;
    model->submit();
}

void QtItemViewController::revert()
{
    Q_D(QtItemViewController);
    QAbstractItemModel* model = d->itemView->model();
    if (!model)
        return;
    model->revert();
}

void QtItemViewController::enableActions()
{
    Q_D(QtItemViewController);

    QAbstractItemModel* model = d->itemView->model();
    if (!model)
        return;

    QAction* act = 0;
    for (int i = 0, k = 1; i < QtItemViewControllerPrivate::MAX_ROLE; k <<= 1, ++i)
    {
        if (!d->flags.testFlag(static_cast<ActionRole>(k)))
            continue;

        act = d->actionMap[i];
        if (act == Q_NULLPTR)
            continue;

        switch(k)
        {
        case CommitRole:
        case RevertRole:
        case RemoveRole:
            act->setEnabled(model->rowCount() > 0);
            break;
        case MoveUpRole:
        case MoveDownRole:
            act->setEnabled(model->rowCount() > 1);
            break;
        }
    }
}

