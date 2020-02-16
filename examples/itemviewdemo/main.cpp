#include "demowidget.h"
#include <QApplication>
#include <QDir>

#include <QtPluginManager>
#include <QtPluginInterface>

#include <QtTableModelExporterPlugin>
#include <QtTableModelExporterFactory>

class QtItemModelExporterInterface :
        public QtGenericInterface<QtTableModelExporterPlugin>
{
    typedef QtGenericInterface<QtTableModelExporterPlugin> InterfaceBase;
    // QtPluginInterface interface
public:
    QtItemModelExporterInterface() :
        InterfaceBase(QObject::tr("Model Export")) {
    }

    bool resolve(QObject *instance) const Q_DECL_OVERRIDE {
        QtTableModelExporterPlugin* plugin = qobject_cast<QtTableModelExporterPlugin*>(instance);
        if (plugin) {
            QtTableModelExporterFactory::instance()->registerExporter(plugin);
            return true;
        }
        return false;
    }
};

QT_PLUGIN_INTERFACE(QtItemModelExporterInterface)

void loadPlugins()
{
    QtPluginManager& manager = QtPluginManager::instance();

    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    manager.load(pluginsDir);
    manager.load();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loadPlugins();

    DemoWidget w;
    w.show();

    return a.exec();
}
