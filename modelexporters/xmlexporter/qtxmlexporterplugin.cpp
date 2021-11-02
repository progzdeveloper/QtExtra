#include "qtxmlexporterplugin.h"
#include "qtxmlexporter.h"

QtXmlExporterPlugin::QtXmlExporterPlugin(QObject *parent) :
    QObject(parent)
{
}

QtTableModelExporter* QtXmlExporterPlugin::create( QAbstractTableModel* model ) const
{
    return new QtTableModelXmlExporter(model);
}

QString QtXmlExporterPlugin::exporterName() const
{
    return QStringLiteral("XML");
}

QIcon QtXmlExporterPlugin::icon() const
{
    return QIcon(":/images/export-xml");
}

#if QT_VERSION < 0x050000
#ifdef _DEBUG
Q_EXPORT_PLUGIN2("xmlexporterd", QtXmlExporterPlugin)
#else
Q_EXPORT_PLUGIN2("xmlexporter", QtXmlExporterPlugin)
#endif
#endif // QT_VERSION < 0x050000
