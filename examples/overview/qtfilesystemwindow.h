#ifndef QTTREEWINDOW_H
#define QTTREEWINDOW_H

#include <QWidget>

class QTreeView;
class QFileSystemModel;
class QtOverviewWidget;

class QtFileSystemWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QtFileSystemWindow(QWidget *parent = nullptr);
    ~QtFileSystemWindow();

private:
    QFileSystemModel* model;
    QTreeView* view;
    QtOverviewWidget* overview;

};

#endif // QTTREEWINDOW_H
