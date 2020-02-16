#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QtCrumbBar>


class QMenu;
class QAction;
class QTreeView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

public slots:
    void updateActions();

private slots:
    void insertChild();
    bool insertColumn();
    void insertRow();
    bool removeColumn();
    void removeRow();
    void changeValue(const QString& name, const QVariant& value);

private:
    void setupUi();

private:
    QtCrumbBar* breadcrumbBar;
    QTreeView* view;
    QMenu* fileMenu;
    QAction* exitAction;
    QMenu* actionsMenu;
    QAction* removeRowAction;
    QAction* removeColumnAction;
    QAction* insertRowAction;
    QAction* insertColumnAction;
    QAction* insertChildAction;
};


class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget* parent = 0);
private Q_SLOTS:
    void formatPath();
private:
    QTreeView* view;
};

#endif // MAINWINDOW_H
