#include "qtfilesystemwindow.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <QFileSystemModel>

#include <QtOverviewWidget>

static int wCount = 0;

static inline void createCornerButton(QAbstractScrollArea* area, QtOverviewWidget* overview)
{
    QPushButton* cornerButton = new QPushButton(area);
    cornerButton->setCheckable(true);
    cornerButton->setChecked(overview->isVisible());
    QObject::connect(cornerButton, SIGNAL(toggled(bool)), overview, SLOT(setVisible(bool)));
    area->setCornerWidget(cornerButton);
}


QtFileSystemWindow::QtFileSystemWindow(QWidget *parent) : QWidget(parent)
{
    view = new QTreeView(this);
    model = new QFileSystemModel(view);
    view->setModel(model);

    view->setAnimated(true);
    view->setHorizontalScrollMode(QTreeView::ScrollPerPixel);
    view->setVerticalScrollMode(QTreeView::ScrollPerPixel);

    view->setAlternatingRowColors(true);
    view->setSelectionBehavior(QTreeView::SelectRows);
    view->setSelectionMode(QTreeView::ExtendedSelection);

    overview = new QtOverviewWidget(Q_NULLPTR, Qt::WindowStaysOnTopHint);
    overview->setArea(view);
    overview->hide();

    createCornerButton(view, overview);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(view);
    //layout->addWidget(overview);

    setWindowTitle(tr("File View %1").arg(++wCount));

    model->setRootPath(QDir::homePath());
}

QtFileSystemWindow::~QtFileSystemWindow()
{
    --wCount;
}
