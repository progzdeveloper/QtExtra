#include "qtjsonexporterplugin.h"
#include "qtjsonexporter.h"

QtJsonExporterPlugin::QtJsonExporterPlugin(QObject *parent) :
     QObject(parent)
{
}

QtTableModelExporter *QtJsonExporterPlugin::create(QAbstractTableModel *model) const
{
    return new QtTableModelJsonExporter(model);
}

QString QtJsonExporterPlugin::exporterName() const
{
    return QStringLiteral("JSON");
}

QIcon QtJsonExporterPlugin::icon() const
{
    return QIcon(":/images/export-json");
}



#if QT_VERSION < 0x050000
#ifdef _DEBUG
Q_EXPORT_PLUGIN2("jsonexporterd", QtJsonExporterPlugin)
#else
Q_EXPORT_PLUGIN2("jsonexporter", QtJsonExporterPlugin)
#endif
#endif
