#include "qtcsvexportplugin.h"
#include "qtcsvexporter.h"


QtCsvExporterPlugin::QtCsvExporterPlugin( QObject *parent /*= 0*/ ) :
    QObject(parent)
{
}

QtTableModelExporter* QtCsvExporterPlugin::create( QAbstractTableModel* model ) const
{
    return new QtTableModelCsvExporter(model);
}

QString QtCsvExporterPlugin::exporterName() const
{
    return tr("CSV");
}

QIcon QtCsvExporterPlugin::icon() const
{
    return QIcon(":/images/export-csv");
}

#if QT_VERSION < 0x050000
#ifdef _DEBUG
Q_EXPORT_PLUGIN2("csvexporterd", QtCsvExporterPlugin)
#else
Q_EXPORT_PLUGIN2("csvexporter", QtCsvExporterPlugin)
#endif
#endif
