#ifndef MODELWIDGET_H
#define MODELWIDGET_H

#include <QtWidgets>

class QSqlTableModel;
class QSqlQueryModel;
class QtTreeProxyModel;

class ModelWidget : public QWidget
{
    Q_OBJECT

public:
    ModelWidget(QWidget *parent = 0);
    ~ModelWidget();

private:
    QTreeView* view;
    QSqlQueryModel* model;
    QtTreeProxyModel* proxy;
};

#endif // MODELWIDGET_H
