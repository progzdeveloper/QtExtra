#ifndef QTTABLEMODELEXPORTERPLUGIN_H
#define QTTABLEMODELEXPORTERPLUGIN_H

#include <QtWidgetsExtra>
#include <QtPlugin>
#include <QString>
#include <QIcon>

class QAbstractTableModel;
class QtTableModelExporter;

class QTWIDGETSEXTRA_EXPORT QtTableModelExporterPlugin
{
public:
    virtual ~QtTableModelExporterPlugin(){}
    virtual QtTableModelExporter* create(QAbstractTableModel* model) const = 0;
    virtual QString exporterName() const = 0;
    virtual QIcon icon() const = 0;
};

Q_DECLARE_INTERFACE(QtTableModelExporterPlugin, "com.QtExtra.QtTableModelExporterPlugin/1.0")

#endif

