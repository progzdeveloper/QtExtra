#pragma once
#include <QtGlobal>
#include <QtTableModelExporterPlugin>


class QtExcelExporterPlugin :
        public QObject,
        public QtTableModelExporterPlugin
{
    Q_OBJECT
    Q_INTERFACES(QtTableModelExporterPlugin)
    Q_CLASSINFO("Version", "1.0 (Windows OS Only)")

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.QtExtra.QtTableModelExporterPlugin/1.0" FILE "excelexporter.json")

#endif // QT_VERSION >= 0x050000

public:
    explicit QtExcelExporterPlugin(QObject *parent = Q_NULLPTR);
    // QtTableModelExporterPlugin interface
    QtTableModelExporter* create(QAbstractTableModel* model) const;
    QString exporterName() const;
    QIcon icon() const;
};

