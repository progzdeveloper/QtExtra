#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>

class QtVariantListModel;
class QtCheckableProxyModel;
class QtRevertibleProxyModel;
class QtItemViewController;

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

public Q_SLOTS:
    void updateActions(const QModelIndex&, int);
    void updateActions();
    void checkedItems();
private:
    QtItemViewController* controller;
    QtVariantListModel* model;
    QtCheckableProxyModel* viewProxy;
    QtRevertibleProxyModel* cachingProxy;
};

#endif // WIDGET_H
