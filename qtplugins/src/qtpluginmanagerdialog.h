#pragma once
#include <QDialog>

#include <QtPluginsExt>

class QTreeWidgetItem;

class QtPluginManagerDialogPrivate;
class QTPLUGINS_EXPORT QtPluginManagerDialog :
        public QDialog
{
    Q_OBJECT
public:
    explicit QtPluginManagerDialog(QWidget *parent = Q_NULLPTR);
    ~QtPluginManagerDialog();

private Q_SLOTS:
    void showProperties(QTreeWidgetItem* item);

private:
    QScopedPointer<class QtPluginManagerDialogPrivate> d;
};


