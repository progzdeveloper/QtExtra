#include "qtcsvexporter.h"
#include <QTextStream>
#include <QTextCodec>
#include <QVariant>
#include <QDateTime>
#include <QCoreApplication>
#include <QtPropertyWidget>
#include <QDialog>


QT_METAINFO_TR(QtTableModelCsvExporter)
{
    QT_TR_META("QtTableModelCsvExporterPrivate", "CSV Export"),
    QT_TR_META("QtTableModelCsvExporterPrivate", "Field delimiter"), // Разделитель полей
    QT_TR_META("QtTableModelCsvExporterPrivate", "String delimiter"), // Разделитель строк
    QT_TR_META("QtTableModelCsvExporterPrivate", "Date format"), // Формат даты
    QT_TR_META("QtTableModelCsvExporterPrivate", "Time format"), // Формат времени
    QT_TR_META("QtTableModelCsvExporterPrivate", "Boolean alpha"), // Булево значение текстом
};


class QtTableModelCsvExporterPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtTableModelCsvExporterPrivate)
public:
    QtTableModelCsvExporter *q_ptr;
    bool boolalpha;
    QString delimiter;
    QChar stringQuote;
    QString dateFormat;
    QString timeFormat;
    QTextStream stream;

    inline void storeTableHeader();
    inline void storeItem(const QVariant& v);
};



void QtTableModelCsvExporterPrivate::storeTableHeader()
{
    QAbstractTableModel *m = q_ptr->model();
    QString s;
    for (int i = 0; i < m->columnCount(); ++i) {
        s = m->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        stream << QStringLiteral("?%1?").replace('?', stringQuote).arg(s) << delimiter;
    }

}

void QtTableModelCsvExporterPrivate::storeItem( const QVariant& v )
{
    switch(v.type())
    {
    case QVariant::Bool:
    {
        bool b = v.toBool();
        if (boolalpha)
            stream << (b ? "true" : "false");
        else
            stream << static_cast<int>(b);
    }
        break;
    case QVariant::Date:
        stream << (v.toDate().toString(dateFormat));
        break;
    case QVariant::Time:
        stream << (v.toTime().toString(timeFormat));
        break;
    case QVariant::DateTime:
        stream <<  (v.toDateTime().toString(dateFormat + " " + timeFormat));
        break;
    case QVariant::Double:
        stream << v.toDouble();
        break;
    case QVariant::LongLong:
        stream << v.toLongLong();
        break;
    case QVariant::ULongLong:
        stream << v.toULongLong();
        break;
    case QVariant::Int:
        stream << v.toInt();
        break;
    case QVariant::UInt:
        stream << v.toUInt();
        break;
    case QVariant::ByteArray:
    case QVariant::String:
    default:
        stream << QString("?%1?").replace('?', stringQuote).arg(v.toString());
    }

}





QtTableModelCsvExporter::QtTableModelCsvExporter(QAbstractTableModel* model, const QString& delim) :
    QtTableModelExporter(model),
    d_ptr(new QtTableModelCsvExporterPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->boolalpha = false;
    d_ptr->delimiter = delim;
    d_ptr->stringQuote = '"';
    d_ptr->dateFormat = "dd-MM-yyyy";
    d_ptr->timeFormat = "hh.mm.ss";
    setModel(model);
}

QtTableModelCsvExporter::~QtTableModelCsvExporter()
{
    delete d_ptr;
}


void QtTableModelCsvExporter::setDelimiter(const QString& delim)
{
    Q_D(QtTableModelCsvExporter);
    d->delimiter = delim;
}
QString QtTableModelCsvExporter::delimiter() const
{
    Q_D(const QtTableModelCsvExporter);
    return d->delimiter;
}

void QtTableModelCsvExporter::setBoolAlpha(bool on)
{
    Q_D(QtTableModelCsvExporter);
    d->boolalpha = on;
}

bool QtTableModelCsvExporter::isBoolAlpha() const
{
    Q_D(const QtTableModelCsvExporter);
    return d->boolalpha;
}

void QtTableModelCsvExporter::setStringQuote(QChar ch)
{
    Q_D(QtTableModelCsvExporter);
    d->stringQuote = ch;
}
QChar QtTableModelCsvExporter::stringQuote() const
{
    Q_D(const QtTableModelCsvExporter);
    return d->stringQuote;
}

void QtTableModelCsvExporter::setDateFormat(const QString& ch)
{ 
    Q_D(QtTableModelCsvExporter);
    d->dateFormat = ch;
}

QString QtTableModelCsvExporter::dateFormat() const
{
    Q_D(const QtTableModelCsvExporter);
    return d->dateFormat;
}

void QtTableModelCsvExporter::setTimeFormat(const QString& ch)
{
    Q_D(QtTableModelCsvExporter);
    d->timeFormat = ch;
}

QString QtTableModelCsvExporter::timeFormat() const
{
    Q_D(const QtTableModelCsvExporter);
    return d->timeFormat;
}

QStringList QtTableModelCsvExporter::fileFilter() const
{
    return (QStringList() << tr("Plain text (*.csv *.txt *.tab)"));
}

bool QtTableModelCsvExporter::exportModel(QIODevice *device)
{
    Q_D(QtTableModelCsvExporter);
    if (!beginExport(device))
        return false;

    d->stream.setDevice(device);
    d->stream.setCodec(textCodec());

    if (isHeaderStored()) {
        d->storeTableHeader();
        d->stream << endl;
    }
    storeIndex();
    endExport();
    return (d->stream.status() == QTextStream::Ok);
}

void QtTableModelCsvExporter::storeIndex(const QModelIndex& index)
{
    Q_D(QtTableModelCsvExporter);

    if (aborted())
        return;

    if (index.isValid()) {
        d->storeItem(index.data(itemRole()));
        d->stream << d->delimiter;
        return;
    }

    if (d->stream.status() != QTextStream::Ok) {
        setErrorString(d->stream.device()->errorString());
        return;
    }

    const QAbstractTableModel *m = model();
    const int rowCount =  m->rowCount(index);
    const int columnCount =  m->columnCount(index);
    int step = 0;
    for (int r = 0; r < rowCount; ++r) {
        for (int c = 0; c < columnCount; ++c) {
            storeIndex(m->index(r, c, index));
            setProgress(++step);
        }
        d->stream << endl; // write EOL after every row
    }
}

QWidget *QtTableModelCsvExporter::createEditor(QDialog *parent) const
{
    QtPropertyWidget* editor = new QtPropertyWidget(parent);
    editor->setObject(const_cast<QtTableModelCsvExporter*>(this));
    editor->setClassFilter(QStringLiteral("^(?!QObject$).*"));
    QObject::connect(parent, SIGNAL(accepted()), editor, SLOT(submit()));
    QObject::connect(parent, SIGNAL(rejected()), editor, SLOT(revert()));
    return editor;
}

