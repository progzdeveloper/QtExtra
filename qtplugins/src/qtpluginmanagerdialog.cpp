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
    QtPluginManagerDialog *q;
    QDialogButtonBox *buttonBox;
    QTableView *propertyView;
    QtPluginPropertyModel *propertyModel;
    QtPluginTreeView *pluginView;

    QtPluginManagerDialogPrivate(QtPluginManagerDialog* dialog);
    void initUi();
};

QtPluginManagerDialogPrivate::QtPluginManagerDialogPrivate(QtPluginManagerDialog *dialog) :
    q(dialog) {
}

void QtPluginManagerDialogPrivate::initUi()
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, q);
    QObject::connect(buttonBox, SIGNAL(accepted()), q, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), q, SLOT(reject()));

    pluginView = new QtPluginTreeView(q);
    pluginView->setHeaderHidden(true);
    propertyView = new QTableView(q);
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
                     q, SLOT(showProperties(QTreeWidgetItem*)));
    QObject::connect(pluginView, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
                     q, SLOT(showProperties(QTreeWidgetItem*)));

    QSplitter *splitter = new QSplitter;
    splitter->addWidget(pluginView);
    splitter->addWidget(propertyView);

    QVBoxLayout *mainLayout = new QVBoxLayout(q);
    mainLayout->addWidget(splitter);
    mainLayout->addWidget(buttonBox);
}



QtPluginManagerDialog::QtPluginManagerDialog( QWidget *parent /*= 0*/ )
    : QDialog(parent), d(new QtPluginManagerDialogPrivate(this))
{
    d->initUi();
}

QtPluginManagerDialog::~QtPluginManagerDialog()
{
}

void QtPluginManagerDialog::showProperties( QTreeWidgetItem* item )
{
    d->propertyModel->clear();
    const QString key = d->pluginView->key(d->pluginView->indexFromItem(item));
    if (key.isEmpty())
        return;
    d->propertyModel->setMetadata(QtPluginManager::instance().metadata(key));
}
