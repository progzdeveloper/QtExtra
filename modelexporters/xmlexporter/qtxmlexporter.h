#pragma once
#include <QtTableModelExporter>

class QtTableModelXmlExporter :
        public QtTableModelExporter
{
    Q_OBJECT
    Q_CLASSINFO("QtTableModelXmlExporter", "XML Export")

    Q_PROPERTY(bool autoFormat READ isAutoFormatting WRITE setAutoFormatting)
    Q_CLASSINFO("autoFormat", "AutoFormat")

public:
    explicit QtTableModelXmlExporter(QAbstractTableModel* model = Q_NULLPTR);
    ~QtTableModelXmlExporter();

    void setAutoFormatting(bool on = true);
    bool isAutoFormatting() const;

    // QtTableModelExporter interface
    QStringList fileFilter() const override;
    bool exportModel(QIODevice *device) override;
    void storeIndex(const QModelIndex& index = QModelIndex()) override;
    QWidget *createEditor(QDialog *parent) const override;

private:
    class QtTableModelXmlExporterPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtTableModelXmlExporter)
};
