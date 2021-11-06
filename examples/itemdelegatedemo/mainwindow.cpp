#include "mainwindow.h"
#include "treemodel.h"

#include <QFile>
#include <QtWidgets>

#include <QtItemModelUtility>

#include <QtSheetItemDelegate>
#include <QtCounterItemDelegate>
#include <QtCompositeItemDelegate>
#include <QtRichTextItemDelegate>
#include <QtMenuItemDelegate>
#include <QtComboBoxItemDelegate>
#include <QtEditDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();

    QStringList headers;
    headers << tr("Title") << tr("Description");

    QFile file(":/default.txt");
    file.open(QIODevice::ReadOnly);
    TreeModel *model = new TreeModel(headers, file.readAll());
    file.close();

    QtCompositeItemDelegate* delegate = new QtCompositeItemDelegate(view);
    new QtMenuItemDelegate(delegate);
    new QtSheetItemDelegate(view, delegate);
    new QtCounterItemDelegate(delegate);
    new QtRichTextItemDelegate(delegate);



    view->setItemDelegate(delegate);
    //view->setItemDelegateForColumn(1, new QtMenuItemDelegate(view));
    //view->setItemDelegateForColumn(1, new QtRichTextItemDelegate(view));
    //view->setItemDelegateForColumn(0, new QtSheetItemDelegate(view, view));
    view->setEditTriggers(QTreeView::AnyKeyPressed);
    view->setUniformRowHeights(false);
    view->setVerticalScrollMode(QTreeView::ScrollPerPixel);
    view->setAnimated(true);
    view->setRootIsDecorated(true);

    view->setModel(model);
    breadcrumbBar->setModel(view->model());


    for (int column = 0; column < model->columnCount(); ++column) {
        view->resizeColumnToContents(column);
    }
    view->header()->setMinimumSectionSize(view->header()->sectionSize(0));


    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateActions);

    connect(actionsMenu, &QMenu::aboutToShow, this, &MainWindow::updateActions);
    connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
    connect(insertColumnAction, &QAction::triggered, this, &MainWindow::insertColumn);
    connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
    connect(removeColumnAction, &QAction::triggered, this, &MainWindow::removeColumn);
    connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);

    updateActions();

    statusBar()->showMessage(tr("Ready"), 5000);
}

void MainWindow::setupUi()
{
    menuBar()->setNativeMenuBar(false);
    fileMenu = menuBar()->addMenu(tr("File"));
    exitAction = fileMenu->addAction(tr("Exit"), this, &MainWindow::close);

    actionsMenu = menuBar()->addMenu(tr("Actions"));
    exitAction = actionsMenu->addAction(tr("Exit"));

    removeRowAction = actionsMenu->addAction(tr("Remove Row"));
    removeColumnAction = actionsMenu->addAction(tr("Remove Column"));
    insertRowAction = actionsMenu->addAction(tr("Insert Row"));
    insertColumnAction = actionsMenu->addAction(tr("Insert Column"));
    insertChildAction = actionsMenu->addAction(tr("Insert Child"));

    view = new QTreeView(this);

    breadcrumbBar = new QtCrumbBar(this);
    breadcrumbBar->setCompleter(Q_NULLPTR);

    connect(view, SIGNAL(activated(QModelIndex)), breadcrumbBar, SLOT(setCurrentIndex(QModelIndex)));
    //connect(view, SIGNAL(pressed(QModelIndex)), breadcrumbBar, SLOT(enterTree(QModelIndex)));
    //connect(view, SIGNAL(expanded(QModelIndex)), breadcrumbBar, SLOT(enterTree(QModelIndex)));
    connect(breadcrumbBar, SIGNAL(indexChanged(QModelIndex)), view, SLOT(setCurrentIndex(QModelIndex)));

    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(breadcrumbBar);
    layout->addWidget(view);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    setCentralWidget(widget);
}


void MainWindow::insertChild()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    QtEditDialog dialog(this);
    dialog.registerEditor<QVariant::String, QLineEdit>("text");
    connect(&dialog, SIGNAL(valueChanged(QString,QVariant)), SLOT(changeValue(QString,QVariant)));
    for (int column = 0; column < model->columnCount(index); ++column) {
        QString title = model->headerData(column, Qt::Horizontal).toString();
        dialog.insert(title, title, QString("%1%2").arg(title).arg(column));
    }
    if (dialog.exec() == QDialog::Rejected)
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, column, index);
        //model->setData(child, QVariant("[No data]"), Qt::EditRole);
        QString title = model->headerData(column, Qt::Horizontal).toString();
        model->setData(child, dialog.value(title), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);
    }

    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}

bool MainWindow::insertColumn()
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);

    updateActions();

    return changed;
}

void MainWindow::insertRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    QtEditDialog dialog(this);
    dialog.registerEditor<QVariant::String, QLineEdit>("text");
    connect(&dialog, SIGNAL(valueChanged(QString,QVariant)), SLOT(changeValue(QString,QVariant)));
    for (int column = 0; column < model->columnCount(index); ++column) {
        QString title = model->headerData(column, Qt::Horizontal).toString();
        dialog.insert(title, title, QString("%1%2").arg(title).arg(column));
    }
    if (dialog.exec() == QDialog::Rejected)
        return;

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        QString title = model->headerData(column, Qt::Horizontal).toString();
        model->setData(child, dialog.value(title), Qt::EditRole);
        //model->setData(child, QVariant("[No data]"), Qt::EditRole);
    }
}

bool MainWindow::removeColumn()
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column);

    if (changed)
        updateActions();

    return changed;
}

void MainWindow::removeRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void MainWindow::changeValue(const QString &name, const QVariant &value)
{
    statusBar()->showMessage(QString("Value Changed['%1': %2]").arg(name, value.toString()));
}


void MainWindow::updateActions()
{
    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    removeRowAction->setEnabled(hasSelection);
    removeColumnAction->setEnabled(hasSelection);

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    insertRowAction->setEnabled(hasCurrent);
    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        int row = view->selectionModel()->currentIndex().row();
        int column = view->selectionModel()->currentIndex().column();
        if (view->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}






Widget::Widget(QWidget *parent) : QWidget(parent)
{
    QDirModel* model = new QDirModel(this);

    view = new QTreeView(this);
    view->setModel(model);

    QtCrumbBar* crumbBar = new QtCrumbBar(this);
    crumbBar->setModel(model);
    crumbBar->setRootIcon(view->style()->standardIcon(QStyle::SP_ComputerIcon));

    crumbBar->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* action = new QAction(tr("Copy..."));
    connect(action, SIGNAL(triggered()), SLOT(formatPath()));
    crumbBar->addAction(action);

    connect(view, SIGNAL(activated(QModelIndex)), crumbBar, SLOT(setCurrentIndex(QModelIndex)));
    connect(view, SIGNAL(expanded(QModelIndex)), crumbBar, SLOT(setCurrentIndex(QModelIndex)));
    connect(crumbBar, SIGNAL(indexChanged(QModelIndex)), view, SLOT(setCurrentIndex(QModelIndex)));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(crumbBar, 0);
    layout->addWidget(view, 1);
}

void Widget::formatPath()
{
    qDebug() << ::formatPath(view->currentIndex());
}


