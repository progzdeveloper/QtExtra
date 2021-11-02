#pragma once
#include <QObject>
#include <QtTableModelExporterPlugin>

class QtXmlExporterPlugin:
        public QObject,
        public QtTableModelExporterPlugin
{
    Q_OBJECT
    Q_CLASSINFO("Version", "1.0")
    Q_CLASSINFO("QtXmlExporterPlugin", "XML Export Plugin")

    Q_INTERFACES(QtTableModelExporterPlugin)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.QtExtra.QtTableModelExporterPlugin/1.0" FILE "xmlexporter.json")
#endif // QT_VERSION >= 0x050000

public:
    explicit QtXmlExporterPlugin(QObject *parent = Q_NULLPTR);

    // QtTableModelExporterPlugin interface
    QtTableModelExporter* create(QAbstractTableModel* model) const;
    QString exporterName() const;
    QIcon icon() const;
};

