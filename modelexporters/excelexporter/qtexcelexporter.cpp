#include "qtexcelexporter.h"
#ifdef Q_OS_WIN
#include <QDir>
#include <QFile>
#include <QAxObject>
#include <QScopedPointer>
#include <QMessageBox>
#include <QTextCodec>
#include <QDateTime>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>

#include <QtPropertyWidget>

#define MAX_COLUMNS 700


static const char * ExcelExporterMetaInfoTr [] =
{
    QT_TRANSLATE_NOOP("QtTableModelExcelExporter", "Microsoft Excel Export"),
    QT_TRANSLATE_NOOP("QtTableModelExcelExporter", "Date format"), // Формат даты
    QT_TRANSLATE_NOOP("QtTableModelExcelExporter", "Time format"), // Формат времени
    QT_TRANSLATE_NOOP("QtTableModelExcelExporter", "Open MS Excel"), // Открыть MS Exel
};


class QtTableModelExcelExporterPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtTableModelExcelExporterPrivate)
public:
    QtTableModelExcelExporter *q_ptr;
    QString dateFormat, timeFormat;
    bool autoOpen;
    QScopedPointer<QAxObject> workbook;
    QScopedPointer<QAxObject> activeSheet;
    QScopedPointer<QAxObject> sheetCells;
    QScopedPointer<QAxObject> excel;

    QtTableModelExcelExporterPrivate(QtTableModelExcelExporter *q);
    void storeHeader();
    inline QString columnName(int column) const;
    inline QString rowName(int row) const;
    inline QVariant formatValue( const QVariant& v );
    inline void openExcel(const QString& fileName);
};

QtTableModelExcelExporterPrivate::QtTableModelExcelExporterPrivate(QtTableModelExcelExporter* q) :
    q_ptr(q),
    dateFormat("dd.MM.yyyy"),
    timeFormat("hh:mm:ss"),
    autoOpen(false)
{
}

void QtTableModelExcelExporterPrivate::storeHeader()
{
    QAbstractTableModel * m = q_ptr->model();
    for (int section = 0; section < m->columnCount(); section++) {
        sheetCells->dynamicCall(
                    "SetItem(QVariant, QVariant, QVariant)",
                    rowName(0),
                    columnName(section),
                    formatValue(m->headerData(section, Qt::Horizontal, Qt::DisplayRole))
                    );
    }
}

QString QtTableModelExcelExporterPrivate::columnName( int col ) const
{
    QString s;
    int alpha, remainder;
    alpha = col / 26;
    remainder = col % 26;
    if (alpha > 0) {
        s += QChar(alpha-1 + 'A');
    }
    if (remainder >= 0) {
        s += QChar(remainder+'A');
    }
    return s;
}

QString QtTableModelExcelExporterPrivate::rowName( int row ) const
{
    return QString::number(row+1);
}


QVariant QtTableModelExcelExporterPrivate::formatValue( const QVariant& v )
{
    switch(v.type())
    {
    case QVariant::ByteArray:
    case QVariant::String:
        return q_ptr->textCodec()->toUnicode(v.toByteArray());
    case QVariant::Date:
        return (v.toDate().toString(dateFormat));
    case QVariant::Time:
        return (v.toTime().toString(timeFormat));
    case QVariant::DateTime:
        return (v.toDateTime().toString(dateFormat + " " + timeFormat));
    default:
        return v;
    }
}

void QtTableModelExcelExporterPrivate::openExcel(const QString& fileName)
{
    if ( !QDesktopServices::openUrl(QUrl::fromLocalFile(fileName)) ) {
        QMessageBox::critical(0,
                              tr("Data Export"),
                              tr("Unable to open file %1.\nFile does not exist or inaccessible."));
    }
}



QtTableModelExcelExporter::QtTableModelExcelExporter( QAbstractTableModel* model /*= 0*/ ) :
    QtTableModelExporter(model),
    d_ptr(new QtTableModelExcelExporterPrivate(this))
{
    QProgressDialog dlg;
    dlg.setLabelText(tr("Connecting with Microsoft Excel..."));
    dlg.setCancelButton(0);

    dlg.setRange(0, 4);

    d_ptr->workbook.reset(new QAxObject("Excel.Sheet"));
    dlg.setValue(1);

    d_ptr->excel.reset(d_ptr->workbook->querySubObject("Application"));
    dlg.setValue(2);

    d_ptr->activeSheet.reset(d_ptr->workbook->querySubObject("ActiveSheet"));
    dlg.setValue(3);

    d_ptr->sheetCells.reset(d_ptr->activeSheet->querySubObject("Cells"));
    dlg.setValue(4);

    connect(d_ptr->workbook.data(), SIGNAL(exception(int, const QString&, const QString&, const QString&)),
            SLOT(reportError(int, const QString&, const QString&, const QString&)));
    connect(d_ptr->activeSheet.data(), SIGNAL(exception(int, const QString&, const QString&, const QString&)),
            SLOT(reportError(int, const QString&, const QString&, const QString&)));
    connect(d_ptr->sheetCells.data(), SIGNAL(exception(int, const QString&, const QString&, const QString&)),
            SLOT(reportError(int, const QString&, const QString&, const QString&)));

}

QtTableModelExcelExporter::~QtTableModelExcelExporter()
{
    delete d_ptr;
}

void QtTableModelExcelExporter::setDateFormat( const QString& format )
{
    Q_D(QtTableModelExcelExporter);
    d->dateFormat = format;
}

QString QtTableModelExcelExporter::dateFormat() const
{
    Q_D(const QtTableModelExcelExporter);
    return d->dateFormat;
}

void QtTableModelExcelExporter::setTimeFormat( const QString& format )
{
    Q_D(QtTableModelExcelExporter);
    d->timeFormat = format;
}

QString QtTableModelExcelExporter::timeFormat() const
{
    Q_D(const QtTableModelExcelExporter);
    return d->timeFormat;
}

void QtTableModelExcelExporter::setAutoOpen(bool on)
{
    Q_D(QtTableModelExcelExporter);
    d->autoOpen = on;
}

bool QtTableModelExcelExporter::isAutoOpen() const
{
    Q_D(const QtTableModelExcelExporter);
    return d->autoOpen;
}

QStringList QtTableModelExcelExporter::fileFilter() const
{
    return (QStringList() << tr("MS Excel Workbook 2007 (*.xlsx)")
            << tr("MS Excel Workbook 97-2003 (*.xls)"));
}

bool QtTableModelExcelExporter::exportModel( QIODevice *device )
{
    Q_D(QtTableModelExcelExporter);
    QFile *file = qobject_cast<QFile*>(device);
    if (!file) {
        setErrorString(tr("Incorrect output device: saving data in this format (MS Excel) can only be done in file."));
        return false;
    }

    if (!d->workbook ||
            !d->activeSheet ||
            !d->sheetCells) {
        setErrorString(tr("Unable to export data: no connection with с Microsoft Excel.\n\
                          Maybe one or more Microsoft Office modules wich is nessesary to export is missed."));
                          return false;
    }

    d->excel->setProperty("DisplayAlerts", false);
    if (!beginExport(device))
        return false;

    if (file->isOpen())
        file->close();

    d->workbook->dynamicCall("SetTitle(QVariant)", tableName());
    d->workbook->dynamicCall("SetName(QVariant)", tableName());
    if (isHeaderStored()) {
        d->storeHeader();
    }
    storeIndex();
    d->workbook->dynamicCall("SaveAs(QVariant)", file->fileName().replace('/', QDir::separator()));
    endExport();

    d->excel->setProperty("DisplayAlerts", true);
    if (d->autoOpen) {
        d->openExcel(file->fileName());
    }

    return true;
}

void QtTableModelExcelExporter::storeIndex( const QModelIndex& index /*= QModelIndex()*/ )
{
    Q_D(QtTableModelExcelExporter);
    if (aborted())
        return;

    const QAbstractTableModel *m = model();
    const int rowCount    = m->rowCount(index);
    const int columnCount = m->columnCount(index);

    if (columnCount > MAX_COLUMNS)
        setErrorString(tr("Unable to export data: incorrect number of columns"));

    if (index.isValid())
    {
        d->sheetCells->dynamicCall(
                    "SetItem(QVariant, QVariant, QVariant)",
                    d->rowName(isHeaderStored() ? index.row()+1 : index.row()),
                    d->columnName(index.column()),
                    d->formatValue(index.data(itemRole()))
                    );
        return;
    }

    int step = 0;
    for (int r = 0; r < rowCount; ++r) {
        for (int c = 0; c < columnCount; ++c) {
            storeIndex(m->index(r, c, index));
            setProgress(++step);
        }
    }
}

QWidget *QtTableModelExcelExporter::createEditor(QDialog *parent) const
{
    QtPropertyWidget* editor = new QtPropertyWidget(parent);
    editor->setObject(const_cast<QtTableModelExcelExporter*>(this));
    editor->setClassFilter("^(?!QObject$).*");
    QObject::connect(parent, SIGNAL(accepted()), editor, SLOT(submit()));
    QObject::connect(parent, SIGNAL(rejected()), editor, SLOT(revert()));
    return editor;
}


void QtTableModelExcelExporter::reportError( int errcode, const QString& source, const QString& desc, const QString& help )
{
    QMessageBox::critical(0, tr("Export Error"), tr("Error #%1\nSource: %2\nDescription: %3\nHelp: %4")
                          .arg(errcode)
                          .arg(source)
                          .arg(desc)
                          .arg(help));
}


#undef MAX_COLUMNS

#endif
