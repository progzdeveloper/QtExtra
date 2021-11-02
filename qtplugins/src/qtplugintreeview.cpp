#include "qtplugintreeview.h"
#include "qtpluginmetadata.h"
#include "qtpluginmanager.h"
#include <QApplication>

class QtPluginItem : 
        public QTreeWidgetItem
{
public:
    enum ItemType
    {
        FolderItem = 0,
        LibraryItem = 1
    };

    QtPluginItem(QTreeWidget *view, int type /* = Type */)
        : QTreeWidgetItem(view, type)
    {}

    QtPluginItem(QTreeWidgetItem *parent, int type /* = Type */)
        : QTreeWidgetItem(parent, type)
    {}

    virtual ~QtPluginItem() {}
};



class QtPluginFolderItem : 
        public QtPluginItem
{
public:
    QtPluginFolderItem(QTreeWidget *view)
        : QtPluginItem(view, QtPluginItem::FolderItem)
    {}

    inline QVariant data(int column, int role) const
    {
        if (column == 0 && role == Qt::DecorationRole) {
            QStyle *style = qApp->style();
            return style->standardIcon( isExpanded() ? QStyle::SP_DirOpenIcon : QStyle::SP_DirClosedIcon );
        }
        return QtPluginItem::data(column, role);
    }
};

class QtPluginLibraryItem : 
        public QtPluginItem
{
public:
    QtPluginLibraryItem(QTreeWidgetItem *parent, const QtPluginMetadata& metadata)
        : QtPluginItem(parent, QtPluginItem::LibraryItem), pluginKey(metadata.key)
    {
        setText(0, metadata.libPath.section("/", -1));
        setIcon(0, QIcon(metadata.isLoaded ? QIcon::fromTheme("user-available", QIcon(":/images/done"))
                                           : QIcon::fromTheme("user-offline", QIcon(":/images/fail"))));
        setToolTip(0, metadata.isLoaded ? QObject::tr("Module was loaded successfully") : metadata.errorString);
    }

    inline QString key() const { return pluginKey; }

private:
    const QString pluginKey;
};



class QtPluginTreeViewPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtPluginTreeViewPrivate)
public:
    QtPluginTreeView *q_ptr;
    QtPluginTreeViewPrivate(QtPluginTreeView* q);
    void buildTree();
};


QtPluginTreeViewPrivate::QtPluginTreeViewPrivate(QtPluginTreeView *q) :
    q_ptr(q) {
}

void QtPluginTreeViewPrivate::buildTree()
{
    QtPluginFolderItem *folder = 0;
    QtPluginManager& manager = QtPluginManager::instance();

    QStringList iids = manager.iids();
    iids << tr("Unknown");

    for (auto iidIt = iids.cbegin(); iidIt != iids.cend(); ++iidIt)
    {
        folder = new QtPluginFolderItem(q_ptr);
        folder->setText(0, manager.category(*iidIt));
        const QStringList keys = manager.keys(*iidIt);
        auto it = keys.cbegin();
        for (; it != keys.cend(); ++it)
            new QtPluginLibraryItem(folder, manager.metadata(*it));
    }
}



QtPluginTreeView::QtPluginTreeView( QWidget *parent /*= 0*/ ) 
    : QTreeWidget(parent), d_ptr(new QtPluginTreeViewPrivate(this))
{
    d_ptr->buildTree();
}

QtPluginTreeView::~QtPluginTreeView()
{
}

QString QtPluginTreeView::key( const QModelIndex& index ) const
{
    //Q_D(const QtPluginTreeView);
    const QTreeWidgetItem *it = itemFromIndex(index);
    if (it->type() != QtPluginItem::LibraryItem) {
        return QString();
    }
    const QtPluginLibraryItem *item = static_cast<const QtPluginLibraryItem*>(it);
    return (item != 0 ? item->key() : QString());

}
