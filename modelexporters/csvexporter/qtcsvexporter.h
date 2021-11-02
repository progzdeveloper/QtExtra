#pragma once
#include <QtTableModelExporter>

class QTextCodec;

class QtTableModelCsvExporter :
        public QtTableModelExporter
{
    Q_OBJECT
    Q_CLASSINFO("QtTableModelCsvExporter", "CSV Export")

    Q_PROPERTY(QString delimiter READ delimiter WRITE setDelimiter)
    Q_CLASSINFO("delimiter", "Field delimiter")

    Q_PROPERTY(QChar stringQuote READ stringQuote WRITE setStringQuote)
    Q_CLASSINFO("stringQuote", "String delimiter")

    Q_PROPERTY(QString dateFormat READ dateFormat WRITE setDateFormat)
    Q_CLASSINFO("dateFormat", "Date format")

    Q_PROPERTY(QString timeFormat READ timeFormat WRITE setTimeFormat)
    Q_CLASSINFO("timeFormat", "Time format")

    Q_PROPERTY(bool boolAlpha READ isBoolAlpha WRITE setBoolAlpha)
    Q_CLASSINFO("boolAlpha", "Boolean alpha") // Булево значение текстом

public:
    explicit QtTableModelCsvExporter(QAbstractTableModel* model = Q_NULLPTR, const QString& delim = QLatin1String(";"));
    ~QtTableModelCsvExporter();

    void setDelimiter(const QString& delim);
    QString delimiter() const;

    void setBoolAlpha(bool on = true);
    bool isBoolAlpha() const;

    void setStringQuote(QChar ch);
    QChar stringQuote() const;

    void setDateFormat(const QString& ch);
    QString dateFormat() const;

    void setTimeFormat(const QString& ch);
    QString timeFormat() const;

    QStringList fileFilter() const;

    // QtTableModelExporterPlugin interface
    bool exportModel(QIODevice *device);
    void storeIndex(const QModelIndex& index = QModelIndex());
    QWidget *createEditor(QDialog *parent) const;

private:
    class QtTableModelCsvExporterPrivate *d_ptr;
    Q_DISABLE_COPY(QtTableModelCsvExporter)
    Q_DECLARE_PRIVATE(QtTableModelCsvExporter)
};
