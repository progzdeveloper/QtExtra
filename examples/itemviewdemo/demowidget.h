#pragma once
#include <QWidget>

class QAction;
class QTableView;
class QAbstractTableModel;
class QSortFilterProxyModel;

class QtItemViewController;
class QtPatternEdit;

class DemoWidget : public QWidget
{
    Q_OBJECT
public:
    DemoWidget(QWidget* parent = 0);
    ~DemoWidget();

private slots:
    void currentChanged(const QModelIndex& index);
    void filterModel(const QString& pattern);
    void aboutPlugins();
    void exportModel();

private:
    void enableActions();

private:
    QTableView* view;
    QAbstractTableModel* model;
    QSortFilterProxyModel* proxy;
    QtItemViewController* controller;
    QtPatternEdit* filterEdit;
};

