#include <QApplication>
#include <QSortFilterProxyModel>
#include <QtWidgets>
#include <QFile>
#include <QDebug>

#include <QtActionItemDelegate>
#include <QtItemViewController>

#include <QtPluginManagerDialog>

#include <QtTableModelExporter>
#include <QtTableModelExporterDialog>
#include <QtTableModelExporterFactory>

#include <QtItemViewNavigator>

#include <QtCustomHeaderView>
#include <QtPatternEdit>


#include "demowidget.h"
#include "qoptionmodel.h"

DemoWidget::DemoWidget(QWidget* parent /* = 0*/) : QWidget(parent)
{
    view = new QTableView(this);
    model = new QOptionModel(view);
    proxy = new QSortFilterProxyModel(model);
    proxy->setSourceModel(model);
    proxy->setFilterRole(Qt::DisplayRole);
    view->setModel(proxy);
    view->setSelectionMode(QTableView::SingleSelection);
    view->setSelectionBehavior(QTableView::SelectRows);
    view->setHorizontalHeader(new QtCustomHeaderView(Qt::Horizontal, view));
    view->setVerticalHeader(new QtCustomHeaderView(Qt::Vertical, view));
    view->horizontalHeader()->setStretchLastSection(true);
    view->setAlternatingRowColors(true);
    //view->setEditTriggers(QListView::AllEditTriggers);
    view->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(view, SIGNAL(clicked(const QModelIndex&)), SLOT(currentChanged(const QModelIndex&)));
    connect(view, SIGNAL(pressed(const QModelIndex&)), SLOT(currentChanged(const QModelIndex&)));

    QtItemViewNavigator* viewNavigator = new QtItemViewNavigator(view, this);
    viewNavigator->setText(tr("Record:  "));

    filterEdit = new QtPatternEdit(this);
    filterEdit->setPlaceholderText(tr("Filter..."));
    connect(filterEdit, SIGNAL(textChanged(QString)), SLOT(filterModel(QString)));

    QSignalMapper* signalMapper = new QSignalMapper(this);
    QMenu* columnMenu = new QMenu(tr("Filter Column"), this);
    QActionGroup* actionGroup = new QActionGroup(columnMenu);
    for (int section = 0; section < model->columnCount(); section++) {
        QString columnName = model->headerData(section, Qt::Horizontal).toString();
        QAction* action = actionGroup->addAction(columnName);
        action->setCheckable(true);
        action->setChecked(section == proxy->filterKeyColumn());
        connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
        signalMapper->setMapping(action, section);
    }
    actionGroup->setExclusive(true);
    columnMenu->addActions(actionGroup->actions());
    connect(signalMapper, qOverload<int>(&QSignalMapper::mapped),
            proxy, &QSortFilterProxyModel::setFilterKeyColumn);

    controller = new QtItemViewController(view);
    controller->setRoles(QtItemViewController::InsertRole|QtItemViewController::RemoveRole|
                         QtItemViewController::MoveUpRole|QtItemViewController::MoveDownRole);

    QToolBar* toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(16, 16));

    QAction* action = Q_NULLPTR;
    action = controller->action(QtItemViewController::InsertRole);
    connect(action, SIGNAL(triggered()), controller, SLOT(insertItem()));
    toolBar->addAction(action);

    action = controller->action(QtItemViewController::RemoveRole);
    connect(action, SIGNAL(triggered()), controller, SLOT(removeItem()));
    toolBar->addAction(action);

    action = controller->action(QtItemViewController::MoveUpRole);
    connect(action, SIGNAL(triggered()), controller, SLOT(moveItemUp()));
    toolBar->addAction(action);

    action = controller->action(QtItemViewController::MoveDownRole);
    connect(action, SIGNAL(triggered()), controller, SLOT(moveItemDown()));
    toolBar->addAction(action);

    toolBar->addSeparator();
    toolBar->addWidget(viewNavigator);

    toolBar->addSeparator();
    toolBar->addAction(columnMenu->menuAction());
    toolBar->addWidget(filterEdit);


    toolBar->addSeparator();
    toolBar->addAction(tr("Plugins"), this, SLOT(aboutPlugins()));
    toolBar->addAction(tr("Export"), this, SLOT(exportModel()));

    //QtActionItemDelegate* d = controller->createDelegate(view);
    //d->setIconSize(QSize(10, 10));
    //view->setItemDelegate(d);
    view->addActions(controller->actions());


    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(toolBar);
    layout->addWidget(view);

    controller->enableActions();
}

DemoWidget::~DemoWidget()
{
}

void DemoWidget::currentChanged(const QModelIndex &)
{
    controller->enableActions();
}

void DemoWidget::filterModel(const QString &pattern)
{
    proxy->setFilterCaseSensitivity(filterEdit->options() & QtPatternEdit::CaseSensitive ?
                                        Qt::CaseSensitive : Qt::CaseInsensitive);

    if (filterEdit->options() & QtPatternEdit::RegularExpr)
        proxy->setFilterRegExp(pattern);
    else if (filterEdit->options() & QtPatternEdit::WholeWords)
        proxy->setFilterFixedString(pattern);
    else
        proxy->setFilterWildcard(pattern);
}

void DemoWidget::aboutPlugins()
{
    QtPluginManagerDialog dlg;
    dlg.exec();
}

void DemoWidget::exportModel()
{
    QString dialogTitle = tr("Export");
    QString errorTitle = tr("Export Error");
    bool isOk = false;

    QtTableModelExporterFactory* exporterFactory = QtTableModelExporterFactory::instance();
    QStringList keys = exporterFactory->keys();
    std::sort(keys.begin(), keys.end());
    QString key = QInputDialog::getItem(this, dialogTitle,
                                        tr("Select export format: "),
                                        keys, 0, false, &isOk);
    if (key.isEmpty() || !isOk)
        return;


    QScopedPointer<QtTableModelExporter> exporter(exporterFactory->createExporter(key, this->model));
    if (!exporter) {
        QMessageBox::critical(this, errorTitle, tr("Failed to export in '%1' format!").arg(key));
        return;
    }

    QScopedPointer<QDialog> dialog(exporter->createDialog(this));
    dialog->setWindowTitle(dialogTitle);
    if (dialog->exec() == QDialog::Rejected)
        return;

    QString fileFilter = exporter->fileFilter().join(";;");
    QString fileName = QFileDialog::getSaveFileName(this, errorTitle, QString(), fileFilter);
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, errorTitle,
                              tr("Failed to open the file '%1': %2")
                                    .arg(fileName, file.errorString()));
    }

    if (!exporter->exportModel(&file)) {
        QMessageBox::critical(this, errorTitle,
                              exporter->errorString());
    }
}


