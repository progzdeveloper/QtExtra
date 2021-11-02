#pragma once
#include <QtTableModelExporter>
#include <QJsonDocument>

class QTextCodec;

class QtTableModelJsonExporter :
        public QtTableModelExporter
{
    Q_OBJECT
    Q_CLASSINFO("QtTableModelJsonExporter", "JSON Export")

    Q_PROPERTY(QString dateFormat READ dateFormat WRITE setDateFormat)
    Q_CLASSINFO("dateFormat", "Date format")

    Q_PROPERTY(QString timeFormat READ timeFormat WRITE setTimeFormat)
    Q_CLASSINFO("timeFormat", "Time format")

    Q_PROPERTY(bool boolAlpha READ isBoolAlpha WRITE setBoolAlpha)
    Q_CLASSINFO("boolAlpha", "Boolean alpha") // Булево значение текстом

    Q_PROPERTY(Format jsonFormat READ jsonFormat WRITE setJsonFormat)
    Q_CLASSINFO("jsonFormat", "JSON format") // json format type

public:
    enum Format
    {
        Indented = QJsonDocument::Indented,
        Compact  = QJsonDocument::Compact
    };
    Q_ENUM(Format)

    explicit QtTableModelJsonExporter(QAbstractTableModel *model = Q_NULLPTR);
    ~QtTableModelJsonExporter();

    // QtTableModelExporter interface
public:
    void setBoolAlpha(bool on = true);
    bool isBoolAlpha() const;

    void setDateFormat(const QString& ch);
    QString dateFormat() const;

    void setTimeFormat(const QString& ch);
    QString timeFormat() const;

    void setJsonFormat(Format f);
    Format jsonFormat() const;

    QStringList fileFilter() const override;
    bool exportModel(QIODevice *device) override;
    QWidget *createEditor(QDialog *parent) const override;

protected:
    void storeIndex(const QModelIndex &index) override;

private:
    class QtTableModelJsonExporterPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QtTableModelJsonExporter)
};

