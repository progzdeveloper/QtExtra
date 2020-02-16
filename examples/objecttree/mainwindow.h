#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class QTreeView;
class QtObjectTreeModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void createMenus();
    void createActions();
    void createView();

private Q_SLOTS:
    void insertObject();
    void removeObject();


private:
    QObject* rootObject;
    QtObjectTreeModel* model;
    QTreeView* view;
    QMenu* fileMenu;
    QMenu* itemMenu;
};

#endif // MAINWINDOW_H
