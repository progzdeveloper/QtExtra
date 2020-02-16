#include "qtactionitemdelegate.h"
#include <QAction>
#include <QToolButton>
#include <QHBoxLayout>
#include <QAbstractItemView>
#include <QSignalMapper>
#include <QApplication>

#include <QDebug>

#include <QtItemModelUtility>


class QtActionItemDelegatePrivate
{
public:
    QList<QAction*> actions;
    Qt::ToolButtonStyle buttonStyle;
    bool isAutoRaise;
    QSize iconSize;
    QSignalMapper* mapper;

    QToolButton* createButton(QAction* act, QWidget* parent) const;
    void fillLayout(QHBoxLayout* layout, int side, QWidget* parent) const;
};

QToolButton* QtActionItemDelegatePrivate::createButton(QAction* act, QWidget* parent) const
{
    QToolButton* button = new QToolButton(parent);
    button->setToolButtonStyle(buttonStyle);
    button->setAutoRaise(isAutoRaise);
    button->setIconSize(iconSize);
    button->setIcon(act->icon());
    button->setText(act->text());
    button->setToolTip(act->toolTip());
    button->setStatusTip(act->statusTip());
    button->setEnabled(act->isEnabled());
    button->setVisible(act->isVisible());
    button->adjustSize();
    button->setFixedSize(button->size());
    QObject::connect(button, SIGNAL(clicked()), act, SIGNAL(triggered()));
    return button;
}

void QtActionItemDelegatePrivate::fillLayout(QHBoxLayout* layout, int side, QWidget* parent) const
{ 
    QList<QAction*>::const_iterator it = actions.begin();
    for (; it != actions.end(); ++it) {
        if ((*it)->property("side").toInt() == side) {
            QToolButton* button = createButton(*it, parent);
            QObject::connect(*it, SIGNAL(changed()), mapper, SLOT(map()));
            mapper->setMapping(*it, button);
            layout->addWidget(button);
        }
    }
}



QtActionItemDelegate::QtActionItemDelegate(QObject* parent /* = 0*/) :
    QStyledItemDelegate(parent),
    d_ptr(new QtActionItemDelegatePrivate)
{
    d_ptr->buttonStyle = Qt::ToolButtonFollowStyle;
    d_ptr->isAutoRaise = true;
    int s = qApp->style()->pixelMetric(QStyle::PM_ButtonIconSize);
    d_ptr->iconSize = QSize(s, s);
    d_ptr->mapper = new QSignalMapper(this);
};

QtActionItemDelegate::~QtActionItemDelegate()
{
    delete d_ptr;
}

void QtActionItemDelegate::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    Q_D(QtActionItemDelegate);
    d->buttonStyle = style;
}

Qt::ToolButtonStyle QtActionItemDelegate::toolButtonStyle() const
{
    Q_D(const QtActionItemDelegate);
    return d->buttonStyle;
}

void QtActionItemDelegate::setAutoRaise(bool on)
{
    Q_D(QtActionItemDelegate);
    d->isAutoRaise = on;
}

bool QtActionItemDelegate::isAutoRaise() const
{
    Q_D(const QtActionItemDelegate);
    return d->isAutoRaise;
}

void QtActionItemDelegate::setIconSize(const QSize& size)
{
    Q_D(QtActionItemDelegate);
    d->iconSize = size;
}

QSize QtActionItemDelegate::iconSize() const
{
    Q_D(const QtActionItemDelegate);
    return d->iconSize;
}

void QtActionItemDelegate::addActions(const QList<QAction*>& actions, Side side /*= RightSide*/)
{
    for (QList<QAction*>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
        addAction(*it, side);
    }
}

void QtActionItemDelegate::addAction(QAction* action, Side side /*= RightSide*/)
{
    Q_D(QtActionItemDelegate);
    action->setProperty("side", side);
    d->actions << action;
}

void QtActionItemDelegate::removeAction(QAction* action)
{
    Q_D(QtActionItemDelegate);
    d->actions.removeAll(action);
    d->mapper->removeMappings(action);
    QObject::disconnect(action, 0, d->mapper, 0);
}

QList<QAction*> QtActionItemDelegate::actions() const
{
    Q_D(const QtActionItemDelegate);
    return d->actions;
}

QWidget *QtActionItemDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    Q_D(const QtActionItemDelegate);
    QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);

    QtItemEditor* itemEditor = new QtItemEditor(editor, parent);

    QHBoxLayout* layout = new QHBoxLayout(itemEditor);
    layout->setMargin(0);
    layout->setSpacing(3);

    d->fillLayout(layout, LeftSide, itemEditor);
    layout->addWidget( editor );
    layout->addStretch();
    d->fillLayout(layout, RightSide, itemEditor);

    connect(d->mapper, SIGNAL(mapped(QWidget*)), SLOT(updateButton(QWidget*)));
    connect(itemEditor, SIGNAL(destroyed(QObject*)), SLOT(editorDestroyed(QObject*)));

    return itemEditor;
}

void QtActionItemDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    QtItemEditor* itemEditor = qobject_cast<QtItemEditor*>(editor);
    if (itemEditor) {
        QStyledItemDelegate::setEditorData(itemEditor->widget(), index);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void QtActionItemDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
    QtItemEditor* itemEditor = qobject_cast<QtItemEditor*>(editor);
    if (itemEditor) {
        QStyledItemDelegate::setModelData(itemEditor->widget(), model, index);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}


void QtActionItemDelegate::updateButton(QWidget* widget)
{
    Q_D(QtActionItemDelegate);
    QToolButton* button = qobject_cast<QToolButton*>(widget);
    if (!button)
        return;

    QAction* act = qobject_cast<QAction*>(d->mapper->mapping(widget));
    if (!act)
        return;

    button->setIcon(act->icon());
    button->setText(act->text());
    button->setToolTip(act->toolTip());
    button->setStatusTip(act->statusTip());
    button->setEnabled(act->isEnabled());
    button->setVisible(act->isVisible());
}

void QtActionItemDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex &/* index*/) const
{
    editor->setGeometry(option.rect);
}

void QtActionItemDelegate::editorDestroyed(QObject*)
{
    Q_D(QtActionItemDelegate);
    QList<QAction*>::const_iterator it = d->actions.begin();
    for (; it != d->actions.end(); ++it) {
        d->mapper->removeMappings(*it);
        QObject::disconnect(*it, 0, d->mapper, 0);
    }
}

