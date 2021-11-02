#include <QtSql>
#include <QtTreeProxyModel>
#include <QtPropertyWidget>
#include "modelwidget.h"


ModelWidget::ModelWidget(QWidget *parent)
    : QWidget(parent)
{
    view = new QTreeView(this);

    model = new QSqlQueryModel(this);
    model->setQuery("select id, firstname, lastname, date(timestamp) as birthdate from people order by birthdate desc", QSqlDatabase::database(":memory:"));
    //model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //model->select();

    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("First name"));
    model->setHeaderData(2, Qt::Horizontal, tr("Last name"));

    proxy = new QtTreeProxyModel(this);
    proxy->setGroupping(1, Qt::DisplayRole);
    proxy->setSourceModel(model);

    connect(proxy, &QtTreeProxyModel::groupColumnChanged, view, &QTreeView::doItemsLayout);
    connect(proxy, &QtTreeProxyModel::groupNameRoleChanged, view, &QTreeView::doItemsLayout);

    view->setModel(proxy);

    QtPropertyWidget* controller = new QtPropertyWidget(this);
    controller->setObject(proxy);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(view);
    layout->addWidget(controller);
}

ModelWidget::~ModelWidget()
{

}
