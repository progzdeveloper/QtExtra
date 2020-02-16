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

private slots:
    void showProperties(QTreeWidgetItem* item);

private:
    Q_DISABLE_COPY(QtPluginManagerDialog)
    QT_PIMPL(QtPluginManagerDialog)
};


