#include <QObject>
#include "qtplugininterface.h"


QtPluginInterface::~QtPluginInterface()
{
}

const QString &QtPluginInterface::uncategorized()
{
    static QString categoryName(QObject::tr("Uncategorized"));
    return categoryName;
}

