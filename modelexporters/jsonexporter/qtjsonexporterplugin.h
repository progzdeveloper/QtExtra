#pragma once
#include <QObject>
#include <QtTableModelExporterPlugin>

class QtJsonExporterPlugin :
        public QObject,
        public QtTableModelExporterPlugin
{
    Q_OBJECT
    Q_CLASSINFO("Version", "1.0")
    Q_CLASSINFO("QtJsonExporterPlugin", "JSON Export Plugin")

    Q_INTERFACES(QtTableModelExporterPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "qtjsonexporter.json")
#endif // QT_VERSION >= 0x050000

public:
    explicit QtJsonExporterPlugin(QObject *parent = 0);

    // QtTableModelExporterPlugin interface
    QtTableModelExporter *create(QAbstractTableModel *model) const;
    QString exporterName() const;
    QIcon icon() const;
};

