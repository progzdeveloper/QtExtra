#include "widget.h"
#include <QUuid>
#include <QDateTime>

#include <QtWidgets>

#include <QtCheckableProxyModel>
#include <QtCachingProxyModel>

#include <QtVariantListModel>
#include <QtVariantItemDelegate>

#include <QtItemViewController>
#include <QtItemViewNavigator>


Widget::Widget(QWidget *parent)
    : QMainWindow(parent)
{
    typedef QtItemViewController::ActionRole ActionRole;

    //setIconSize(QSize(16, 16));

    model = new QtVariantListModel(this);
    model->setEditable(true);

    cachingProxy = new QtRevertibleProxyModel(model);
    cachingProxy->setSourceModel(model);
    connect(cachingProxy, SIGNAL(changesCached(QModelIndex,int)), SLOT(updateActions(QModelIndex, int)));
    connect(cachingProxy, SIGNAL(accepted()), SLOT(updateActions()));
    connect(cachingProxy, SIGNAL(rejected()), SLOT(updateActions()));

    viewProxy = new QtCheckableProxyModel(model);
    viewProxy->setSourceModel(cachingProxy);
    viewProxy->setFilterRole(Qt::DisplayRole);

    QListView* view = new QListView(this);
    view->setModel(viewProxy);
    view->setItemDelegate(new QtVariantItemDelegate(view));
    setCentralWidget(view);

    controller = new QtItemViewController(view, ActionRole::AllRoles, this);
    connect(controller->action(ActionRole::CommitRole), SIGNAL(triggered()), controller, SLOT(commit()));
    connect(controller->action(ActionRole::RevertRole), SIGNAL(triggered()), controller, SLOT(revert()));
    connect(controller->action(ActionRole::InsertRole), SIGNAL(triggered()), controller, SLOT(insertItem()));
    connect(controller->action(ActionRole::RemoveRole), SIGNAL(triggered()), controller, SLOT(removeItem()));
    connect(controller->action(ActionRole::MoveUpRole), SIGNAL(triggered()), controller, SLOT(moveItemUp()));
    connect(controller->action(ActionRole::MoveDownRole), SIGNAL(triggered()), controller, SLOT(moveItemDown()));
    connect(controller->action(ActionRole::SortAscRole), SIGNAL(triggered()), controller, SLOT(sortAsc()));
    connect(controller->action(ActionRole::SortDescRole), SIGNAL(triggered()), controller, SLOT(sortDesc()));
    controller->enableActions();
    controller->action(ActionRole::CommitRole)->setEnabled(false);
    controller->action(ActionRole::RevertRole)->setEnabled(false);



    QPushButton* button = new QPushButton(tr("Checked Items..."), this);
    connect(button, SIGNAL(clicked()), SLOT(checkedItems()));

    QLineEdit* patternEdit = new QLineEdit(this);
    patternEdit->setPlaceholderText("<Filter>");
    patternEdit->setClearButtonEnabled(true);
    connect(patternEdit, SIGNAL(textChanged(QString)), viewProxy, SLOT(setFilterRegExp(QString)));

    QToolBar* controlBar = new QToolBar(tr("control"), this);
    controlBar->setAllowedAreas(Qt::TopToolBarArea);
    controlBar->addActions(controller->actions());
    controlBar->addSeparator();
    controlBar->addWidget(patternEdit);
    controlBar->addWidget(button);
    controlBar->setIconSize(QSize(16, 16));
    addToolBar(Qt::TopToolBarArea, controlBar);



    QCheckBox* editableCheck = new QCheckBox(tr(" Editable  "), this);
    editableCheck->setChecked(model->isEditable());
    connect(editableCheck, SIGNAL(toggled(bool)), model, SLOT(setEditable(bool)));

    QtItemViewNavigator* navigator = new QtItemViewNavigator(view, this);
    QToolBar* navigationBar = new QToolBar(tr("navigation"), this);
    navigationBar->setAllowedAreas(Qt::BottomToolBarArea);
    navigationBar->addWidget(navigator);
    navigationBar->addSeparator();
    navigationBar->addWidget(editableCheck);
    controlBar->setIconSize(QSize(16, 16));
    //navigationBar->setIconSize(navigator->iconSize());
    addToolBar(Qt::BottomToolBarArea, navigationBar);


    model->insertRows(0, 12);
    model->setData(model->index(0), true);
    model->setData(model->index(1), -100);
    model->setData(model->index(2), 0.5);
    model->setData(model->index(3), 4096LL);
    model->setData(model->index(4), "Text");
    model->setData(model->index(5), QUuid::createUuid());
    model->setData(model->index(6), QColor(Qt::green));
    model->setData(model->index(7), QDate::currentDate());
    model->setData(model->index(8), QTime::currentTime());
    model->setData(model->index(9), QDateTime::currentDateTime());
    model->setData(model->index(10), font());
    model->setData(model->index(11), QKeySequence(QKeySequence::Close));
}

Widget::~Widget()
{

}

void Widget::updateActions(const QModelIndex &, int)
{
    typedef QtItemViewController::ActionRole ActionRole;
    controller->action(ActionRole::CommitRole)->setEnabled(true);
    controller->action(ActionRole::RevertRole)->setEnabled(true);
}

void Widget::updateActions()
{
    typedef QtItemViewController::ActionRole ActionRole;
    controller->action(ActionRole::CommitRole)->setEnabled(false);
    controller->action(ActionRole::RevertRole)->setEnabled(false);
}

void Widget::checkedItems()
{
    QString result;
    for (int i = 0, rows = model->rowCount(); i < rows; i++) {
        QModelIndex index = viewProxy->mapFromSource(cachingProxy->index(i, 0));
        int state = index.data(Qt::CheckStateRole).toInt();
        if (static_cast<Qt::CheckState>(state) == Qt::Checked) {
            result += index.data(Qt::DisplayRole).toString();
            result += '\n';
        }
    }

    QMessageBox::information(this, tr("Selected Items..."), result);
}
