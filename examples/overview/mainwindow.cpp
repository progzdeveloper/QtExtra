#include "mainwindow.h"
#include "qtcodewindow.h"
#include "qtfilesystemwindow.h"

#include <QtWidgets>
#include <QtOverviewWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createCentralWidget();
    createActions();
    createDockWidgets();
    menuBar()->setNativeMenuBar(false);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createCentralWidget()
{
    area = new QMdiArea(this);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setCentralWidget(area);

    overview = new QtOverviewWidget(area);
    overview->setArea(area);
    overview->setUpdatable(true);
}

void MainWindow::createActions()
{
    fileMenu = menuBar()->addMenu(tr("File"));

    codeAct = fileMenu->addAction(tr("New Code Window..."), this, SLOT(createCodeWindow()));
    viewAct = fileMenu->addAction(tr("New File System Window..."), this, SLOT(createFsWindow()));

    fileMenu->addSeparator();

    exitAct = fileMenu->addAction(tr("Exit"), this, SLOT(close()));
}

void MainWindow::createDockWidgets()
{
    QDockWidget* dockWidget = new QDockWidget(tr("Overview"));
    dockWidget->setWidget(overview);
    dockWidget->setFloating(true);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

}

void MainWindow::createCodeWindow()
{
    QtCodeWindow* w = new QtCodeWindow(area);
    if (!w->load()) {
        delete w;
        return;
    }
    QMdiSubWindow* window = area->addSubWindow(w);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
}

void MainWindow::createFsWindow()
{
    QMdiSubWindow* window = area->addSubWindow(new QtFileSystemWindow(area));
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
}
