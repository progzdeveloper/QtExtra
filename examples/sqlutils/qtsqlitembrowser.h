#ifndef QTSQLITEMBROWSER_H
#define QTSQLITEMBROWSER_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QtCounterItemDelegate>

class QTreeView;
class QToolButton;
class QComboBox;

class QtPatternEdit;
class QtSqlItemTreeModel;



class QtSqlItemSortFilterProxyModel :
        public QSortFilterProxyModel
{
    Q_OBJECT
public:
    QtSqlItemSortFilterProxyModel(QObject* parent = Q_NULLPTR);
    ~QtSqlItemSortFilterProxyModel();

    // QSortFilterProxyModel interface
protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;
};




class QtSqlItemBrowserDelegate :
        public QtCounterItemDelegate
{
    Q_OBJECT

    // QStyledItemDelegate interface
public:
    QtSqlItemBrowserDelegate(QObject* parent = Q_NULLPTR);
    ~QtSqlItemBrowserDelegate();

    static QIcon itemIcon(int type);

    // QAbstractItemDelegate interface
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    static QHash<int, QIcon> icons;
};




class QtSqlItemBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit QtSqlItemBrowser(QWidget *parent = nullptr);

public slots:
    void filterItems(const QString& pattern);
    void categoryChanged();

    void createConnection();
    void removeConnection();

    void databaseChanged(int i);
    void tableChanged(int i);

private:
    QToolButton* itemBox;
    QtPatternEdit* searchBox;
    QTreeView* view;

    QtSqlItemTreeModel* model;
    QtSqlItemSortFilterProxyModel* filterProxy;

    QComboBox* databaseBox;
    QComboBox* tablesBox;
    QComboBox* columnsBox;
};

#endif // QTSQLITEMBROWSER_H
