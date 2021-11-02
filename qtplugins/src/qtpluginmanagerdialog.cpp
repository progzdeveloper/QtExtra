#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "qtpluginmanagerdialog.h"
#include "qtpluginmanager.h"
#include "qtpluginmetadata.h"
#include "qtplugintreeview.h"
#include "qtpluginpropertymodel.h"


class QtPluginManagerDialogPrivate
{
public:
    QtPluginManagerDialog *q_ptr;
    QDialogButtonBox *buttonBox;
    QTableView *propertyView;
    QtPluginPropertyModel *propertyModel;
    QtPluginTreeView *pluginView;

    QtPluginManagerDialogPrivate(QtPluginManagerDialog* q);
    void initUi();
};

QtPluginManagerDialogPrivate::QtPluginManagerDialogPrivate(QtPluginManagerDialog *q) :
    q_ptr(q) {
}

void QtPluginManagerDialogPrivate::initUi()
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, q_ptr);
    QObject::connect(buttonBox, SIGNAL(accepted()), q_ptr, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), q_ptr, SLOT(reject()));

    pluginView = new QtPluginTreeView(q_ptr);
    pluginView->setHeaderHidden(true);
    propertyView = new QTableView(q_ptr);
    propertyModel = new QtPluginPropertyModel(propertyView);
    propertyView->setModel(propertyModel);
    propertyView->setAlternatingRowColors(true);
    propertyView->verticalHeader()->hide();
    propertyView->verticalHeader()->setDefaultSectionSize(20);
    propertyView->horizontalHeader()->setStretchLastSection(true);

    /*QObject::connect(pluginView, SIGNAL(activated(const QModelIndex&)),
        q_ptr, SLOT(showProperties(const QModelIndex&)));*/
    /*QObject::connect(pluginView, SIGNAL(activated(const QModelIndex&)),
        q_ptr, SLOT(showProperties(const QModelIndex&)));
    QObject::connect(pluginView, SIGNAL(clicked(const QModelIndex&)),
        q_ptr, SLOT(showProperties(const QModelIndex&)));*/

    QObject::connect(pluginView, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
                     q_ptr, SLOT(showProperties(QTreeWidgetItem*)));
    QObject::connect(pluginView, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
                     q_ptr, SLOT(showProperties(QTreeWidgetItem*)));

    QSplitter *splitter = new QSplitter;
    splitter->addWidget(pluginView);
    splitter->addWidget(propertyView);

    QVBoxLayout *mainLayout = new QVBoxLayout(q_ptr);
    mainLayout->addWidget(splitter);
    mainLayout->addWidget(buttonBox);
}






QtPluginManagerDialog::QtPluginManagerDialog( QWidget *parent /*= 0*/ )
    : QDialog(parent), d_ptr(new QtPluginManagerDialogPrivate(this))
{
    d_ptr->initUi();
}

QtPluginManagerDialog::~QtPluginManagerDialog()
{
}

void QtPluginManagerDialog::showProperties( QTreeWidgetItem* item )
{
    Q_D(QtPluginManagerDialog);

    d->propertyModel->clear();
    const QString key = d->pluginView->key(d->pluginView->indexFromItem(item));
    if (key.isEmpty())
        return;
    d->propertyModel->setMetadata(QtPluginManager::instance().metadata(key));
}
