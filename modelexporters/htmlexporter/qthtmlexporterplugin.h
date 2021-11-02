#pragma once
#include <QObject>
#include <QtTableModelExporterPlugin>

class QtHtmlExporterPlugin :
        public QObject,
        public QtTableModelExporterPlugin
{
    Q_OBJECT
    Q_CLASSINFO("Version", "1.0")
    Q_CLASSINFO("QtHtmlExporterPlugin", "HTML Export Plugin")

    Q_INTERFACES(QtTableModelExporterPlugin)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.QtExtra.QtTableModelExporterPlugin/1.0" FILE "htmlexporter.json")
#endif // QT_VERSION >= 0x050000

public:
    explicit QtHtmlExporterPlugin(QObject *parent = Q_NULLPTR);
    QtTableModelExporter* create(QAbstractTableModel* model) const;
    QString exporterName() const;
    QIcon icon() const;
};

