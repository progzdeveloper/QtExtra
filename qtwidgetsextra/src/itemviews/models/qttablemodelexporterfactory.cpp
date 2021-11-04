#include <QtGlobal>
#include "qttablemodelexporterfactory.h"
#include "qttablemodelexporterplugin.h"
#include "qttablemodelexporter.h"


QtTableModelExporterFactory::QtTableModelExporterFactory()
{
}

QtTableModelExporterFactory::~QtTableModelExporterFactory()
{
    qDeleteAll(m_creatorHash);
}

void QtTableModelExporterFactory::registerExporter(QtTableModelExporterPlugin* plugin)
{
    if (plugin) {
        m_creatorHash[plugin->exporterName()] = plugin;
    }
}

QtTableModelExporter* QtTableModelExporterFactory::createExporter(const QString& exporter,
                                                                  QAbstractTableModel* model) const
{
    CreatorHash::const_iterator it = m_creatorHash.find(exporter);
    if (it == m_creatorHash.end())
        return 0;
    return (*it)->create(model);
}

QIcon QtTableModelExporterFactory::exporterIcon( const QString& exporter ) const
{
    CreatorHash::const_iterator it = m_creatorHash.find(exporter);
    if (it == m_creatorHash.end())
        return QIcon();
    return (*it)->icon();
}

QStringList QtTableModelExporterFactory::keys() const
{
    return m_creatorHash.keys();
}

QtTableModelExporterFactory* QtTableModelExporterFactory::instance()
{
    static QtTableModelExporterFactory globalInstance;
    return &globalInstance;
}



