#include "qtsqlitembrowser.h"

#include <QSqlDatabase>

#include <QBoxLayout>
#include <QFormLayout>

#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QToolButton>
#include <QComboBox>
#include <QTreeView>
#include <QSortFilterProxyModel>

#include <QtSqlItemTreeModel>
#include <QtSqlConnectionDialog>

#include <QtPatternEdit>

#include <QPainter>



QHash<int, QIcon> QtSqlItemBrowserDelegate::icons;

QtSqlItemBrowserDelegate::QtSqlItemBrowserDelegate(QObject *parent) :
    QtCounterItemDelegate(parent)
{
}

QtSqlItemBrowserDelegate::~QtSqlItemBrowserDelegate()
{
}

QIcon QtSqlItemBrowserDelegate::itemIcon(int type)
{
    if (icons.isEmpty()) {
        icons[QtSqlItemTreeModel::Database] = QIcon(":/images/database");
        icons[QtSqlItemTreeModel::SystemTable] = QIcon(":/images/systable");
        icons[QtSqlItemTreeModel::UserTable] = QIcon(":/images/usrtable");
        icons[QtSqlItemTreeModel::View] = QIcon(":/images/view");
        icons[QtSqlItemTreeModel::Field] = QIcon(":/images/field");
    }
    auto it = icons.find(type);
    return (it != icons.end() ? *it : QIcon());
}

void QtSqlItemBrowserDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return QtCounterItemDelegate::paint(painter, option, index);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    int type = index.data(Qt::UserRole).toInt();
    opt.features |= QStyleOptionViewItem::HasDecoration;
    opt.icon = itemIcon(type);

    QtCounterItemDelegate::paint(painter, opt, index);
}

QSize QtSqlItemBrowserDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    return size + QSize(0, 4);
}





QtSqlItemSortFilterProxyModel::QtSqlItemSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

QtSqlItemSortFilterProxyModel::~QtSqlItemSortFilterProxyModel()
{
}

bool QtSqlItemSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // custom behaviour :
    if(filterRegExp().isEmpty()==false)
    {
        QAbstractItemModel* model = sourceModel();
        // get source-model index for current row
        QModelIndex source_index = model->index(source_row, this->filterKeyColumn(), source_parent);
        if(source_index.isValid())
        {
            // if any of children matches the filter, then current index matches the filter as well
            int i, nb = model->rowCount(source_index) ;
            for(i=0; i<nb; ++i)
            {
                if(filterAcceptsRow(i, source_index))
                {
                    return true;
                }
            }
            // check current index itself :
            QString key = model->data(source_index, filterRole()).toString();
            return key.contains(filterRegExp()) ;
        }
    }
    // parent call for initial behaviour
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent) ;
}







QToolButton* createToolButton(QtSqlItemTreeModel::Category category, const QString& name, QtSqlItemBrowser* parent)
{
    QToolButton* button = new QToolButton(parent);
    button->setCheckable(true);
    button->setChecked(true);
    button->setText(name);
    button->setToolTip(name);
    button->setIcon(QtSqlItemBrowserDelegate::itemIcon(category));
    button->setProperty("category", (int)category);
    QObject::connect(button, SIGNAL(toggled(bool)), parent, SLOT(categoryChanged()));
    return button;
}




QtSqlItemBrowser::QtSqlItemBrowser(QWidget *parent) : QWidget(parent)
{
    view = new QTreeView(this);
    view->setAnimated(true);
    view->setHeaderHidden(true);

    model = new QtSqlItemTreeModel(view);
    filterProxy = new QtSqlItemSortFilterProxyModel(view);
    filterProxy->setSourceModel(model);

    view->setModel(filterProxy);

    QtSqlItemBrowserDelegate* delegate = new QtSqlItemBrowserDelegate(view);
    view->setItemDelegate(delegate);

    searchBox = new QtPatternEdit(this);
    searchBox->setPlaceholderText(tr("<Filter>"));
    connect(searchBox, SIGNAL(textChanged(QString)), SLOT(filterItems(QString)));

    databaseBox = new QComboBox(this);
    databaseBox->setModel(model);
    databaseBox->setItemDelegate(delegate);

    tablesBox   = new QComboBox(this);
    tablesBox->setModel(model);
    tablesBox->setItemDelegate(delegate);
    connect(databaseBox, SIGNAL(currentIndexChanged(int)), SLOT(databaseChanged(int)));

    columnsBox  = new QComboBox(this);
    columnsBox->setModel(model);
    columnsBox->setItemDelegate(delegate);
    connect(tablesBox, SIGNAL(currentIndexChanged(int)), SLOT(tableChanged(int)));


    QToolButton* buttonCreate = new QToolButton(this);
    buttonCreate->setIcon(QIcon::fromTheme("list-add"));
    connect(buttonCreate, SIGNAL(clicked()), SLOT(createConnection()));

    QToolButton* buttonRemove = new QToolButton(this);
    buttonRemove->setIcon(QIcon::fromTheme("list-remove"));
    connect(buttonRemove, SIGNAL(clicked()), SLOT(removeConnection()));

    QHBoxLayout* controlLayout = new QHBoxLayout;
    controlLayout->addWidget(buttonCreate);
    controlLayout->addWidget(buttonRemove);
    controlLayout->addWidget(searchBox);
    controlLayout->addWidget(createToolButton(QtSqlItemTreeModel::SystemTable, tr("System Tables"), this));
    controlLayout->addWidget(createToolButton(QtSqlItemTreeModel::UserTable,   tr("User Tables"),   this));
    controlLayout->addWidget(createToolButton(QtSqlItemTreeModel::View,        tr("Views"),         this));
    controlLayout->addWidget(createToolButton(QtSqlItemTreeModel::Field,       tr("Columns"),       this));


    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Database:"), databaseBox);
    formLayout->addRow(tr("Table:"), tablesBox);
    formLayout->addRow(tr("Column:"), columnsBox);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(controlLayout);
    layout->addWidget(view);
    layout->addLayout(formLayout);

    model->refresh();



}

void QtSqlItemBrowser::filterItems(const QString &pattern)
{
    QtPatternEdit::Options opts = searchBox->options();
    filterProxy->setFilterCaseSensitivity(opts & QtPatternEdit::CaseSensitive ?
                                        Qt::CaseSensitive : Qt::CaseInsensitive);

    if (opts & QtPatternEdit::RegularExpr)
        filterProxy->setFilterRegExp(pattern);
    else if (opts & QtPatternEdit::WholeWords)
        filterProxy->setFilterFixedString(pattern);
    else
        filterProxy->setFilterWildcard(pattern);

    view->expandAll();
}

void QtSqlItemBrowser::categoryChanged()
{
    QToolButton* toolButton = qobject_cast<QToolButton*>(sender());
    if (toolButton == Q_NULLPTR)
        return;

    uint c = model->categories();
    uint selected = toolButton->property("category").toInt();
    if (toolButton->isChecked())
       c |= selected;
    else
       c &= ~selected;

    model->setCategories(QtSqlItemTreeModel::Categories(c));
    view->expandAll();
}

void QtSqlItemBrowser::createConnection()
{
    QtSqlConnectionDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QSqlDatabase db = QSqlDatabase::addDatabase(dialog.driverName(), dialog.connectionName());
        db.setDatabaseName(dialog.databaseName());
        db.setUserName(dialog.userName());
        db.setPassword(dialog.password());
        db.setHostName(dialog.hostName());
        db.setPort(dialog.port());
        db.setConnectOptions(dialog.options());
        if (!db.open()) {
            QMessageBox::critical(this, tr("Connection failed"), tr("Failed to connect with database"));
            QSqlDatabase::removeDatabase(db.connectionName());
        } else {
            model->refresh();
        }
    }
}

void QtSqlItemBrowser::removeConnection()
{

}

void QtSqlItemBrowser::databaseChanged(int i)
{
    QModelIndex index = model->index(i, 0);
    tablesBox->setRootModelIndex(index);
    tablesBox->setCurrentIndex(-1);
    columnsBox->setCurrentIndex(-1);
}

void QtSqlItemBrowser::tableChanged(int i)
{
    columnsBox->setRootModelIndex(model->index(i, 0, tablesBox->rootModelIndex()));
    columnsBox->setCurrentIndex(-1);
}




