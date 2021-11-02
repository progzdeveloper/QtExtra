#include "qtxmlexporter.h"
#include <QTextCodec>
#include <QXmlStreamWriter>

#include <QDialog>
#include <QtPropertyWidget>

QT_METAINFO_TR(QtTableModelXmlExporter)
{
    QT_TR_META(QtTableModelXmlExporter, "XML Export"),
    QT_TR_META(QtTableModelXmlExporter, "AutoFormat"), // Автоформат
};


class QScopedXmlStreamElement
{
public:
    inline QScopedXmlStreamElement(QXmlStreamWriter& stream, const QString& qualifiedName)
        : xml(&stream) {
        xml->writeStartElement(qualifiedName);
    }
    inline QScopedXmlStreamElement(QXmlStreamWriter& stream, const QString& nsUri, const QString& name)
        : xml(&stream) {
        xml->writeStartElement(nsUri, name);
    }
    inline ~QScopedXmlStreamElement() {
        xml->writeEndElement();
    }
private:
    QXmlStreamWriter *xml;
};


class QtTableModelXmlExporterPrivate
{
public:
    QXmlStreamWriter xmlWriter;
    QTextCodec *codec;
    bool autoFormat;
    QtTableModelXmlExporterPrivate();
};


QtTableModelXmlExporterPrivate::QtTableModelXmlExporterPrivate() :
    codec(QTextCodec::codecForLocale()), autoFormat(true)
{
}


QtTableModelXmlExporter::QtTableModelXmlExporter(QAbstractTableModel* model) :
    d_ptr(new QtTableModelXmlExporterPrivate)
{
    setModel(model);
}

QtTableModelXmlExporter::~QtTableModelXmlExporter()
{
    delete d_ptr;
}

void QtTableModelXmlExporter::setAutoFormatting(bool on /* = true */)
{
    Q_D(QtTableModelXmlExporter);
    d->autoFormat = on;
}

bool QtTableModelXmlExporter::isAutoFormatting() const
{
    Q_D(const QtTableModelXmlExporter);
    return d->autoFormat;
}

QStringList QtTableModelXmlExporter::fileFilter() const
{
    return (QStringList() << tr("XML Files (*.xml)"));
}

bool QtTableModelXmlExporter::exportModel(QIODevice *device)
{
    Q_D(QtTableModelXmlExporter);

    if (!beginExport(device))
        return false;

    d->xmlWriter.setDevice(device);
    d->xmlWriter.setCodec(textCodec());
    d->xmlWriter.setAutoFormatting(d->autoFormat);
    d->xmlWriter.writeStartDocument();
    {
        QScopedXmlStreamElement tableElem(d->xmlWriter, "table");
        d->xmlWriter.writeAttribute("name", tableName().isEmpty() ? QLatin1String("table") : tableName());
        storeIndex();
    }
    d->xmlWriter.writeEndDocument();

    endExport();
    return true;
}

void QtTableModelXmlExporter::storeIndex(const QModelIndex& index)
{
    Q_D(QtTableModelXmlExporter);

    const QAbstractTableModel *m = model();
    if (index.isValid()) {
        static QRegExp rx("^\\d+");

        QString text;
        QString headerText = m->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
        headerText = headerText.simplified();
        headerText.replace(' ', '_');
        if (rx.indexIn(headerText) != -1)
            text = QString("column%1").arg(index.column());
        else
            text = headerText.isEmpty() ? QStringLiteral("column%1").arg(index.column()) : headerText;

        QScopedXmlStreamElement elem(d->xmlWriter, text);
        d->xmlWriter.writeCharacters(index.data(itemRole()).toString());
        return;
    }

    const int rowCount =  m->rowCount(index);
    const int columnCount =  m->columnCount(index);
    int step = 0;
    for (int r = 0; r < rowCount; ++r) {
        QScopedXmlStreamElement rowElem(d->xmlWriter, "row");
        d->xmlWriter.writeAttribute("id", QString::number(r));
        for (int c = 0; c < columnCount; ++c)
            storeIndex(m->index(r, c, index));
        setProgress(++step);
    }
}

QWidget *QtTableModelXmlExporter::createEditor(QDialog *parent) const
{
    QtPropertyWidget* editor = new QtPropertyWidget(parent);
    editor->setObject(const_cast<QtTableModelXmlExporter*>(this));
    editor->setClassFilter("^(?!QObject$).*");
    QObject::connect(parent, SIGNAL(accepted()), editor, SLOT(submit()));
    QObject::connect(parent, SIGNAL(rejected()), editor, SLOT(revert()));
    return editor;
}
