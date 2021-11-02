#pragma once
#include <QtGlobal>
#ifdef Q_OS_WIN
#include <QtTableModelExporter>

class QTextCodec;
class QtTableModelExcelExporterPrivate;

class QtTableModelExcelExporter :
        public QtTableModelExporter
{
    Q_OBJECT

    Q_CLASSINFO("QtTableModelExcelExporter", "Microsoft Excel Export")

    Q_PROPERTY(QString dateFormat READ dateFormat WRITE setDateFormat)
    Q_CLASSINFO("dateFormat", "Date format")

    Q_PROPERTY(QString timeFormat READ timeFormat WRITE setTimeFormat)
    Q_CLASSINFO("timeFormat", "Time format")

    Q_PROPERTY(bool autoOpen READ isAutoOpen WRITE setAutoOpen)
    Q_CLASSINFO("autoOpen", "Open MS Excel")

public:
    explicit QtTableModelExcelExporter(QAbstractTableModel* model = Q_NULLPTR);
    ~QtTableModelExcelExporter();

    void setDateFormat(const QString& format);
    QString dateFormat() const;

    void setTimeFormat(const QString& format);
    QString timeFormat() const;

    void setAutoOpen(bool on);
    bool isAutoOpen() const;

    QStringList fileFilter() const;

    bool exportModel(QIODevice *device) override;
    void storeIndex(const QModelIndex& index = QModelIndex()) override;
    QWidget *createEditor(QDialog *parent) const override;

private Q_SLOTS:
    void reportError(int errcode, const QString& source, const QString& desc, const QString& help);

private:
    Q_DISABLE_COPY(QtTableModelExcelExporter)
    Q_DECLARE_PRIVATE(QtTableModelExcelExporter)
    QtTableModelExcelExporterPrivate *d_ptr;
};
#endif
