#include "qthtmlexporterplugin.h"
#include "qthtmlexporter.h"

QtHtmlExporterPlugin::QtHtmlExporterPlugin(QObject *parent) :
    QObject(parent)
{
}

QtTableModelExporter *QtHtmlExporterPlugin::create(QAbstractTableModel *model) const
{
    return new QtTableModelHtmlExporter(model);
}

QString QtHtmlExporterPlugin::exporterName() const
{
    return QStringLiteral("HTML");
}

QIcon QtHtmlExporterPlugin::icon() const
{
    return QIcon(":/images/html");
}




#if QT_VERSION < 0x050000
#ifdef _DEBUG
Q_EXPORT_PLUGIN2("htmlexporterd", QtHtmlExporterPlugin)
#else
Q_EXPORT_PLUGIN2("htmlexporter", QtHtmlExporterPlugin)
#endif
#endif // QT_VERSION < 0x050000

