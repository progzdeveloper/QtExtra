#pragma once
#include <QObject>
#include <QtTableModelExporterPlugin>

class QtCsvExporterPlugin :
        public QObject,
        public QtTableModelExporterPlugin
{
    Q_OBJECT
    Q_CLASSINFO("Version", "1.0")
    Q_CLASSINFO("QtCsvExporterPlugin", "CSV Export Plugin")

    Q_INTERFACES(QtTableModelExporterPlugin)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID "com.QtExtra.QtTableModelExporterPlugin/1.0" FILE "qtcsvexporter.json")
#endif

public:
    explicit QtCsvExporterPlugin(QObject *parent = Q_NULLPTR);

    // QtTableModelExporterPlugin interface
    QtTableModelExporter* create(QAbstractTableModel* model) const;
    QString exporterName() const;
    QIcon icon() const;
};
