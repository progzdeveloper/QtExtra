#include "qtexcelexporterplugin.h"
#ifdef Q_OS_WIN32
#include "qtexcelexporter.h"
#else
#include <QMessageBox>
#endif


QtExcelExporterPlugin::QtExcelExporterPlugin( QObject *parent /*= 0*/ ) : QObject(parent)
{
}

QtTableModelExporter* QtExcelExporterPlugin::create( QAbstractTableModel* model ) const
{
#ifndef Q_OS_WIN
    Q_UNUSED(model);
    QMessageBox::critical(0, tr("Export Error"),
                          tr("Data exporting in this format is accesible only on Windows Platform."));
    return Q_NULLPTR;
#else
    return new QtTableModelExcelExporter(model);
#endif
}

QString QtExcelExporterPlugin::exporterName() const
{
    return tr("Microsft Excel");
}

QIcon QtExcelExporterPlugin::icon() const
{
    return QIcon(":/images/export-excel");
}

#if QT_VERSION < 0x050000
#ifdef _DEBUG
Q_EXPORT_PLUGIN2("qexcelexporterd", QtExcelExporterPlugin)
#else
Q_EXPORT_PLUGIN2("qexcelexporter", QtExcelExporterPlugin)
#endif
#endif

