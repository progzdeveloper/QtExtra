#include "qtcrumbbar.h"
#include "../qtitemmodelutility.h"

#include <QApplication>

#include <QEvent>
#include <QKeyEvent>
#include <QFocusEvent>

#include <QCompleter>

#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QLineEdit>
#include <QBoxLayout>

#include <QStyle>
#include <QStylePainter>

#include <QSize>
#include <QFont>

#include <QtAlgorithms>
#include <QtDebug>


class QtCrumbBarPrivate
{
public:
    QIcon rootIcon;
    QModelIndexList crumbs;
    QHBoxLayout* layout;
    QToolBar* toolBar;
    QLineEdit* lineEdit;

    QAbstractItemModel* model;
    QtCrumbBar* q_ptr;

    QtCrumbBarPrivate(QtCrumbBar* q);

    void initUi();
    void addCrumb(const QModelIndex& index);
};


QtCrumbBarPrivate::QtCrumbBarPrivate(QtCrumbBar *q) :
    layout(Q_NULLPTR),
    toolBar(Q_NULLPTR),
    lineEdit(Q_NULLPTR),
    model(Q_NULLPTR),
    q_ptr(q)
{
}

void QtCrumbBarPrivate::initUi()
{

    QStyle* style = q_ptr->style();
    if (style == Q_NULLPTR)
        style = qApp->style();

    rootIcon = style->standardIcon(QStyle::SP_DirHomeIcon);

    toolBar = new QToolBar(q_ptr);
    toolBar->setMinimumHeight(16);
    toolBar->setIconSize(QSize(16, 16));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->setFloatable(false);
    toolBar->setContentsMargins(0, 0, 0, 0);
    toolBar->layout()->setContentsMargins(0, 0, 0, 0);
    toolBar->layout()->setSpacing(0);

    lineEdit = new QLineEdit(q_ptr);
    lineEdit->hide();
    QObject::connect(lineEdit, SIGNAL(editingFinished()), q_ptr, SLOT(closeEditor()));

    QCompleter* completer = new QCompleter(lineEdit);
    lineEdit->setCompleter(completer);

    QToolButton* backButton = new QToolButton(toolBar);
    backButton->setIconSize(toolBar->iconSize());
    backButton->setIcon(style->standardPixmap(QStyle::SP_ArrowBack));
    backButton->setAutoRaise(true);
    backButton->setFocusPolicy(Qt::NoFocus);
    QObject::connect(backButton, SIGNAL(clicked()), q_ptr, SLOT(back()));

    layout = new QHBoxLayout(q_ptr);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(toolBar, 1);
    layout->addWidget(backButton, 0);
}

void QtCrumbBarPrivate::addCrumb(const QModelIndex &index)
{
    QIcon icon;
    QString text, tooltip;

    QAction* action = Q_NULLPTR;

    if (!model) {
        action = toolBar->addAction("", q_ptr, SLOT(crumbClicked()));
        action->setData(index);
        return;
    }



    int count = model->rowCount(index);
    QMenu* menu = (count > 0 ? new QMenu(toolBar) : Q_NULLPTR);
    for (int i = 0; i < count; ++i) {
        QModelIndex child = model->index(i, 0, index);
        icon = child.data(Qt::DecorationRole).value<QIcon>();
        text = child.data(Qt::DisplayRole).value<QString>();
        tooltip = child.data(Qt::ToolTipRole).value<QString>();
        QtCrumbBar* view = q_ptr;
        action = menu->addAction(icon, text, [child, view]() {
            view->setCurrentIndex(child);
        });
        action->setToolTip(tooltip);
    }

    icon = index.data(Qt::DecorationRole).value<QIcon>();
    text = index.data(Qt::DisplayRole).value<QString>();
    tooltip = index.data(Qt::ToolTipRole).value<QString>();
    tooltip = tooltip.isEmpty() ? text : tooltip;

    if (icon.isNull() && !index.parent().isValid()) { // root index
        icon = rootIcon;
    }

    if (menu != Q_NULLPTR) {
        action = menu->menuAction();
        action->setText(text);
        action->setToolTip(tooltip);
        action->setIcon(icon);
    } else {
        action = toolBar->addAction(text);
        action->setIcon(icon);
        action->setToolTip(tooltip);
    }
    action->setData(index);
    QObject::connect(action, SIGNAL(triggered()), q_ptr, SLOT(crumbClicked()));

    toolBar->addAction(action);

    crumbs.append(index);
}





QtCrumbBar::QtCrumbBar(QWidget *parent) :
    QFrame(parent), d_ptr(new QtCrumbBarPrivate(this))
{
    Q_D(QtCrumbBar);
    d->initUi();

    d->toolBar->installEventFilter(this);
    d->lineEdit->installEventFilter(this);

    setAutoFillBackground(false);
    setBackgroundRole(QPalette::Window);
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFixedHeight(d->toolBar->iconSize().height() + 4);
}

QtCrumbBar::~QtCrumbBar()
{
}

void QtCrumbBar::setModel(QAbstractItemModel *model)
{
    Q_D(QtCrumbBar);
    reset();
    d->model = model;
    d->addCrumb(QModelIndex());
    if (d->lineEdit->completer()) {
        d->lineEdit->completer()->setModel(d->model);
    }
}

QAbstractItemModel *QtCrumbBar::model() const
{
    Q_D(const QtCrumbBar);
    return d->model;
}

QModelIndexList QtCrumbBar::indexList() const
{
    Q_D(const QtCrumbBar);
    return d->crumbs;
}

void QtCrumbBar::setCompleter(QCompleter *completer)
{
    Q_D(QtCrumbBar);
    d->lineEdit->setCompleter(completer);
    if (d->model != Q_NULLPTR)
        d->lineEdit->completer()->setModel(d->model);
}

QCompleter *QtCrumbBar::completer() const
{
    Q_D(const QtCrumbBar);
    return d->lineEdit->completer();
}

void QtCrumbBar::setRootIcon(const QIcon &icon)
{
    Q_D(QtCrumbBar);
    d->rootIcon = icon;
    QList<QAction*> actions = d->toolBar->actions();
    if (!actions.isEmpty()) {
        QAction* rootAct = d->toolBar->actions().front();
        rootAct->setIcon(icon);
    }
}

QIcon QtCrumbBar::rootIcon() const
{
    Q_D(const QtCrumbBar);
    return d->rootIcon;
}

void QtCrumbBar::reset()
{
    Q_D(QtCrumbBar);
    d->crumbs.clear();
    d->toolBar->clear();
}

void QtCrumbBar::back()
{
    Q_D(QtCrumbBar);
    if(d->crumbs.count() <= 1)
        return;

    d->toolBar->removeAction(d->toolBar->actions().last());
    d->crumbs.removeLast();
    Q_EMIT indexChanged(d->crumbs.last());
}

void QtCrumbBar::setCurrentIndex(const QModelIndex &index)
{
    Q_D(QtCrumbBar);
    if(d->model == Q_NULLPTR)
        return;

    if(!d->model->hasChildren(index)) {
        Q_EMIT indexChanged(index);
        return;
    }

    QModelIndex rootIndex;
    if(index == rootIndex) {
        if (!d->toolBar->actions().isEmpty()) {
            d->crumbs.clear();
            d->toolBar->clear();
        }
        d->addCrumb(rootIndex);
    } else {
        d->crumbs.clear();
        d->toolBar->clear();

        d->addCrumb(rootIndex);

        QList<QModelIndex> chain;
        QModelIndex pos = index;
        while(pos.isValid()) {
            chain.append(pos);
            pos = pos.parent();
        }
        while(!chain.isEmpty()) {
            d->addCrumb(chain.last());
            chain.removeLast();
        }
    }
    Q_EMIT indexChanged(index);
}

void QtCrumbBar::crumbClicked()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action != Q_NULLPTR) {
        setCurrentIndex(action->data().value<QModelIndex>());
    }
}

void QtCrumbBar::closeEditor()
{
    Q_D(QtCrumbBar);
    if (d->lineEdit->completer() != Q_NULLPTR) {
        QModelIndex index = d->lineEdit->completer()->currentIndex();
        const QAbstractProxyModel* proxyModel = qobject_cast<const QAbstractProxyModel*>(index.model());
        if (proxyModel)
            setCurrentIndex(proxyModel->mapToSource(index));
    }
    d->toolBar->setVisible(true);
    d->lineEdit->setVisible(false);
    d->layout->replaceWidget(d->lineEdit, d->toolBar, Qt::FindDirectChildrenOnly);
    qApp->processEvents();
}


bool QtCrumbBar::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(QtCrumbBar);
    if (d->lineEdit->completer() != Q_NULLPTR &&
        watched == d->toolBar &&
        event->type() == QEvent::MouseButtonDblClick)
    {
        d->toolBar->setVisible(false);
        d->lineEdit->setVisible(true);
        d->layout->replaceWidget(d->toolBar, d->lineEdit, Qt::FindDirectChildrenOnly);

        QString text;
        if (!d->crumbs.empty())
            text = d->lineEdit->completer()->pathFromIndex(d->crumbs.last());

        d->lineEdit->setText(text);
        d->lineEdit->setFocus();
    }
    if (watched == d->lineEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            closeEditor();
        }
    }

    if (watched == d->lineEdit && event->type() == QEvent::FocusOut) {
        closeEditor();
    }

    return QObject::eventFilter(watched, event);
}

