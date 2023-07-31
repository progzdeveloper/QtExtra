#pragma once
#include <QTreeWidget>

#include <QtPluginsExt>

class QTreeWidgetItem;

class QtPluginTreeViewPrivate;

class QTPLUGINS_EXPORT QtPluginTreeView :
        public QTreeWidget
{
    Q_OBJECT
public:
    explicit QtPluginTreeView(QWidget *parent = Q_NULLPTR);
    ~QtPluginTreeView();

    QString key(const QModelIndex& index) const;

    inline QModelIndex indexFromItem ( QTreeWidgetItem * item, int column = 0 ) const
    {
        return QTreeWidget::indexFromItem(item, column);
    }

private:
    QScopedPointer<class QtPluginTreeViewPrivate> d;
};

