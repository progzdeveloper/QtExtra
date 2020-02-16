#include "mainwindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTreeView>
#include <QInputDialog>
#include <QApplication>

#include <QtObjectTreeModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), rootObject(Q_NULLPTR)
{
    createView();
    createMenus();
    createActions();
}

MainWindow::~MainWindow()
{
}


void MainWindow::createMenus()
{
    menuBar()->setNativeMenuBar(false);
    fileMenu = menuBar()->addMenu(tr("File"));
    itemMenu = menuBar()->addMenu(tr("Items"));
}

void MainWindow::createActions()
{
    fileMenu->addAction(tr("Exit"), qApp, SLOT(quit()));
    itemMenu->addAction(tr("Insert..."), this, SLOT(insertObject()));
    itemMenu->addAction(tr("Remove..."), this, SLOT(removeObject()));
}

void MainWindow::createView()
{
    rootObject = new QObject;
    rootObject->setObjectName(tr("root"));

    view = new QTreeView(this);
    model = new QtObjectTreeModel(this, view);

    view->setModel(model);

    setCentralWidget(view);
}

void MainWindow::insertObject()
{
    QString text = QInputDialog::getText(this, tr("New Object..."), tr("Object Name: "));
    if (text.isEmpty())
        return;

    QModelIndex index = view->currentIndex();
    QObject* parent = model->object(index);
    QObject* object = new QObject(parent == Q_NULLPTR ? rootObject : parent);
    object->setObjectName(text);
}

void MainWindow::removeObject()
{

}
